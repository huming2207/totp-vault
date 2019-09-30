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
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_manager::wifi_evt_handler, this));

    // Register IP events
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, wifi_manager::ip_evt_handler, nullptr));
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

esp_err_t wifi_manager::get_ap_config(wifi_config_t &config)
{
    return esp_wifi_get_config(ESP_IF_WIFI_AP, &config);
}

esp_err_t wifi_manager::get_sta_config(wifi_config_t &config)
{
    return esp_wifi_get_config(ESP_IF_WIFI_STA, &config);
}

void wifi_manager::wifi_evt_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if(arg == nullptr) return;
    auto *wifi_mgr = static_cast<wifi_manager*>(arg);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        auto* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGD(TAG, "Station " MACSTR " joined, AID=%d", MAC2STR(event->mac), event->aid);
        if(wifi_mgr->on_connected_cb) {
            wifi_mgr->on_connected_cb(
                    { event->mac[0], event->mac[1], event->mac[2], event->mac[4], event->mac[5] }, event->aid);
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        auto* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGD(TAG, "Station " MACSTR " left, AID=%d", MAC2STR(event->mac), event->aid);
        if(wifi_mgr->on_disconnected_cb) {
            wifi_mgr->on_disconnected_cb(
                    { event->mac[0], event->mac[1], event->mac[2], event->mac[4], event->mac[5] }, event->aid);
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if(wifi_mgr->on_sta_disconnect_cb) wifi_mgr->on_sta_disconnect_cb();
        if (wifi_manager::sta_retry_cnt < wifi_manager::sta_max_retry) {
            esp_wifi_connect();
            wifi_manager::sta_retry_cnt++;
            ESP_LOGD(TAG, "WiFi connection lost or failed, retry connecting to the AP...");
        }

        ESP_LOGE(TAG,"Connect to the AP failed, maximum retry exceeded");
        if(wifi_mgr->on_sta_connect_lost_cb) wifi_mgr->on_sta_connect_lost_cb();
    }
}

void wifi_manager::ip_evt_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
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
}
