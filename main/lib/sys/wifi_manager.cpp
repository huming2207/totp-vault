#include <cstring>

#include <esp_log.h>

#include <nvs_flash.h>
#include "wifi_manager.hpp"

#define TAG "wifi_mgr"

using namespace sys;

uint8_t wifi_manager::sta_max_retry = 5;
ip_addr_t wifi_manager::ip_addr = {};
ip6_addr_t wifi_manager::ip6_addr = {};
uint8_t wifi_manager::sta_retry_cnt = 0;
EventGroupHandle_t wifi_manager::wifi_event_group = nullptr;

wifi_manager::wifi_manager()
{
    // nvs_flash_init seems to be fine if run more than once in one instance
    auto ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // WiFi event group init
    wifi_manager::wifi_event_group = xEventGroupCreate();
    if(wifi_manager::wifi_event_group == nullptr) {
        ESP_LOGI(TAG, "Failed to initialise WiFi event group!");
        return;
    }

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register WiFi event handler
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
            [] (void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
       if (event_id == WIFI_EVENT_AP_STACONNECTED) {

           auto* event = (wifi_event_ap_staconnected_t*) event_data;
           ESP_LOGI(TAG, "Station " MACSTR " joined, AID=%d",
                   MAC2STR(event->mac), event->aid);
           xEventGroupSetBits(wifi_manager::wifi_event_group, def::WIFI_AP_DEVICE_CONNECT);

       } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {

           auto* event = (wifi_event_ap_stadisconnected_t*) event_data;
           ESP_LOGI(TAG, "Station " MACSTR " left, AID=%d",
                   MAC2STR(event->mac), event->aid);
           xEventGroupSetBits(wifi_manager::wifi_event_group, def::WIFI_AP_DEVICE_DISCONNECT);

       } if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
           esp_wifi_connect();
       } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
           if (wifi_manager::sta_retry_cnt < wifi_manager::sta_max_retry) {
               esp_wifi_connect();
               xEventGroupClearBits(wifi_manager::wifi_event_group, def::WIFI_STA_CONNECTED);
               wifi_manager::sta_retry_cnt++;
               ESP_LOGI(TAG, "WiFi connection lost or failed, retry connecting to the AP...");
           }

           ESP_LOGE(TAG,"Connect to the AP failed, maximum retry exceeded");
           xEventGroupSetBits(wifi_manager::wifi_event_group, def::WIFI_STA_CONNECT_RETRY_MAXIMUM);
       }
    }, nullptr));

    // Register IP events
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID,
        [] (void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
            if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
               auto* event = (ip_event_got_ip_t*) event_data;
               ESP_LOGI(TAG, "Got IPv4 from AP: %s", ip4addr_ntoa(&event->ip_info.ip));
               std::memcpy(&wifi_manager::ip_addr, &event->ip_info.ip, sizeof(ip_addr_t));
               sta_retry_cnt = 0;
               xEventGroupSetBits(wifi_manager::wifi_event_group, def::WIFI_STA_CONNECTED);

            } else if (event_base == IP_EVENT && event_id == IP_EVENT_GOT_IP6) {
               auto* event = (ip_event_got_ip6_t*) event_data;
               ESP_LOGI(TAG, "Got IPv6 from AP: %s", ip6addr_ntoa(&event->ip6_info.ip));
               std::memcpy(&wifi_manager::ip6_addr, &event->ip6_info.ip, sizeof(ip6_addr_t));
               sta_retry_cnt = 0;
               xEventGroupSetBits(wifi_manager::wifi_event_group, def::WIFI_STA_CONNECTED);

            }
       }, nullptr));
}

esp_err_t wifi_manager::set_ap_config(const std::string &ssid, const std::string &passwd, uint8_t channel)
{
    wifi_config_t wifi_config{};
    wifi_config.ap.authmode = passwd.empty() ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;
    wifi_config.ap.beacon_interval = 100;
    wifi_config.ap.channel = channel;
    wifi_config.ap.max_connection = 4;
    std::strcpy(reinterpret_cast<char *>(wifi_config.ap.ssid), ssid.c_str());
    std::strcpy(reinterpret_cast<char *>(wifi_config.ap.password), passwd.c_str());
    wifi_config.ap.ssid_len = ssid.size();

    auto ret = esp_wifi_set_mode(WIFI_MODE_AP);
    ret = ret ?: esp_wifi_set_storage(WIFI_STORAGE_FLASH);
    ret = ret ?: esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    return ret;
}

esp_err_t wifi_manager::set_sta_config(const std::string &ssid, const std::string &passwd, bool fast_scan)
{
    wifi_config_t wifi_config{};
    std::strcpy(reinterpret_cast<char *>(wifi_config.sta.ssid), ssid.c_str());
    std::strcpy(reinterpret_cast<char *>(wifi_config.sta.password), passwd.c_str());
    wifi_config.sta.scan_method = fast_scan ? WIFI_FAST_SCAN : WIFI_ALL_CHANNEL_SCAN;

    auto ret = esp_wifi_set_mode(WIFI_MODE_STA);
    ret = ret ?: esp_wifi_set_storage(WIFI_STORAGE_FLASH);
    ret = ret ?: esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    return ret;
}

esp_err_t wifi_manager::start(wifi_mode_t mode)
{
    auto ret = esp_wifi_set_mode(mode);
    ret = ret ?: esp_wifi_start();
    return ret;
}



wifi_manager& wifi_manager::get_manager()
{
    static wifi_manager instance;
    return instance;
}

esp_err_t wifi_manager::stop()
{
    return esp_wifi_stop();
}

esp_err_t wifi_manager::enable_smoke_signal()
{
    const uint8_t vendor_oui[] = { 0xca, 0xfe, 0xfe };

    // Construct vendor IE
    size_t idf_ver_len = strlen(esp_get_idf_version()) + 1;

    def::smoke_signal payload{};
    payload.element_id = WIFI_VENDOR_IE_ELEMENT_ID;
    payload.length = sizeof(def::smoke_signal);
    payload.vendor_oui_type = 2;
    std::memcpy(payload.vendor_oui, vendor_oui, sizeof(vendor_oui));
    esp_efuse_mac_get_default(payload.mac);
    std::memcpy(payload.idf_ver, esp_get_idf_version(), std::min(idf_ver_len, (size_t)32));

    // Register payload
    auto ret = esp_wifi_set_vendor_ie(true, WIFI_VND_IE_TYPE_BEACON, WIFI_VND_IE_ID_0, &payload);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set vendor IE, ret: %u", ret);
        return ret;
    }

    return ret;
}

esp_err_t wifi_manager::recv_smoke_signal(time_t timeout)
{
    ESP_ERROR_CHECK(esp_wifi_set_vendor_ie_cb([]
    (void *ctx, wifi_vendor_ie_type_t type, const uint8_t sa[6], const vendor_ie_data_t *vnd_ie, int rssi) {
        ESP_LOGI(TAG, "Got Vendor IE from source: " MACSTR ", RSSI: %d dbm", MAC2STR(sa), rssi);
    }, this));


    return 0;
}

esp_err_t wifi_manager::get_ap_config(wifi_config_t &config)
{
    return esp_wifi_get_config(ESP_IF_WIFI_AP, &config);
}

esp_err_t wifi_manager::get_sta_config(wifi_config_t &config)
{
    return esp_wifi_get_config(ESP_IF_WIFI_STA, &config);
}

esp_err_t wifi_manager::send_action_frame(esp_interface_t interface,
                                        const std::vector<uint8_t> &payload, const std::array<uint8_t, 6>& recv_addr,
                                        const std::array<uint8_t, 6>& bssid, const std::array<uint8_t, 3>& oui,
                                        uint8_t repeat)
{
    uint8_t tx_mac[6] = { 0 };
    esp_efuse_mac_get_default(tx_mac);

    def::mgmt_frame_header header;
    header.frame_type = def::FRAME_CTRL_MGMT_ACTION;
    header.frame_flag = 0,
    header.duration = 0;
    std::memcpy(header.receiver_mac, recv_addr.data(), recv_addr.size());
    std::memcpy(header.transmitter_mac, tx_mac, sizeof(tx_mac));
    std::memcpy(header.bssid, bssid.data(), bssid.size());
    header.seq_frag = 0; // Placeholder for now, the WiFi PHY will fill in the correct value by itself

    std::vector<uint8_t> action_pld;

    // Insert Action frame header
    action_pld.insert(action_pld.end(), header.val, header.val + sizeof(def::mgmt_frame_header));

    // Append Action frame type
    action_pld.emplace_back(0x7f); // Vendor-specific Action frame

    // Append OUI
    action_pld.insert(action_pld.end(), std::make_move_iterator(oui.begin()), std::make_move_iterator(oui.end()));

    // Append actual data payload
    action_pld.insert(action_pld.end(),
            std::make_move_iterator(payload.begin()),
            std::make_move_iterator(payload.end()));

    // Send off the buffer with repeating
    auto ret = ESP_OK;
    for(uint8_t idx = 0; idx < repeat; idx++) {
        ret = ret ?: esp_wifi_80211_tx(interface, action_pld.data(), action_pld.size(), true);
    }

    return ret;
}
