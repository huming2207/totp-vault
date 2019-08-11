#include <cstring>
#include <algorithm>

#include <esp_log.h>

#include "wifi_mac.hpp"

#define TAG "wifi_mac"

using namespace sys;

EventGroupHandle_t wifi_mac::wifi_mac_event = nullptr;

wifi_mac::wifi_mac()
{
    // Initialise event group
    wifi_mac_event = xEventGroupCreate();
    if(wifi_mac_event == nullptr) abort();

    // Initialise transmitter addr
    uint8_t default_addr[6] = {0 };
    esp_efuse_mac_get_default(default_addr);

    tx_addr = { MAC2STR(default_addr) };
}

wifi_mac& wifi_mac::get_instance()
{
    static wifi_mac mac;
    return mac;
}

esp_err_t wifi_mac::set_smoke_signal(def::device_config_state state)
{
    // Construct vendor IE
    size_t idf_ver_len = strlen(esp_get_idf_version()) + 1;

    def::smoke_signal payload{};
    payload.element_id = WIFI_VENDOR_IE_ELEMENT_ID;
    payload.length = sizeof(def::smoke_signal);
    payload.vendor_oui_type = 2;
    std::memcpy(payload.vendor_oui, oui.data(), oui.size());
    esp_efuse_mac_get_default(payload.mac);
    std::memcpy(payload.idf_ver, esp_get_idf_version(), std::min(idf_ver_len, (size_t)32));
    payload.state = state;

    // Register payload
    auto ret = esp_wifi_set_vendor_ie(true, WIFI_VND_IE_TYPE_BEACON, WIFI_VND_IE_ID_0, &payload);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set vendor IE, ret: %u", ret);
        return ret;
    }

    return ret;
}

esp_err_t wifi_mac::unset_smoke_signal()
{
    auto ret = esp_wifi_set_vendor_ie(false, WIFI_VND_IE_TYPE_BEACON, WIFI_VND_IE_ID_0, nullptr);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unset vendor IE, ret: %u", ret);
        return ret;
    }

    return ret;
}

wifi_mac& wifi_mac::start_send_action_frame(esp_interface_t interface, const std::vector<uint8_t> &payload)
{
    xEventGroupSetBits(wifi_mac_event, WIFI_MAC_ACTION_STARTED);

    xTaskCreate([](void *ptr){

        auto *mac_class = static_cast<wifi_mac *>(ptr);

        def::mgmt_frame_header header;

        // Initialise action header
        header.frame_type = def::FRAME_CTRL_MGMT_ACTION;
        header.frame_flag = 0;
        header.duration = 0;
        header.seq_frag = 0; // Placeholder for now, the WiFi PHY will fill in the correct value by itself
        std::memcpy(header.receiver_mac, mac_class->rx_addr.data(), mac_class->rx_addr.size());
        std::memcpy(header.transmitter_mac, mac_class->tx_addr.data(), mac_class->tx_addr.size());
        std::memcpy(header.bssid, mac_class->bssid_addr.data(), mac_class->bssid_addr.size());

        std::vector<uint8_t> frame;

        // Insert Action frame header
        frame.insert(frame.end(), header.val, header.val + sizeof(def::mgmt_frame_header));

        // Append Action frame type
        frame.emplace_back(0x7f); // Vendor-specific Action frame

        // Append OUI
        frame.insert(frame.end(),
                     std::make_move_iterator(mac_class->oui.begin()),
                     std::make_move_iterator(mac_class->oui.end()));

        // Append actual data payload
        frame.insert(frame.end(),
                     std::make_move_iterator(mac_class->action_payload.begin()),
                     std::make_move_iterator(mac_class->action_payload.end()));

        auto ret = ESP_OK;
        while((xEventGroupGetBits(wifi_mac_event) & WIFI_MAC_ACTION_STARTED) == WIFI_MAC_ACTION_STARTED) {
            ret = esp_wifi_80211_tx(mac_class->curr_interface, frame.data(), frame.size(), true);
            if(ret != ESP_OK) {
                ESP_LOGE(TAG, "Error occured when sending action frame: %d, now quitting...", ret);
                xEventGroupClearBits(wifi_mac_event, WIFI_MAC_ACTION_STARTED);
            }

            vTaskDelay(pdMS_TO_TICKS(150));
        }

        vTaskDelete(nullptr); // Kill itself if signal is gone
    }, "mac_action", 3072, this, tskIDLE_PRIORITY + 5, nullptr);

    // Send off the buffer with repeating
    return *this;
}

wifi_mac& wifi_mac::stop_send_action_frame()
{
    // Ignore request if the action frame sender thread has started
    if((xEventGroupGetBits(wifi_mac_event) & WIFI_MAC_ACTION_STARTED) == WIFI_MAC_ACTION_STARTED) return *this;
    xEventGroupClearBits(wifi_mac_event, WIFI_MAC_ACTION_STARTED);
    return *this;
}

wifi_mac &wifi_mac::set_receiver_addr(const std::array<uint8_t, 6> &_rx_addr)
{
    // Ignore setting request if the action frame sender thread has started
    if((xEventGroupGetBits(wifi_mac_event) & WIFI_MAC_ACTION_STARTED) == WIFI_MAC_ACTION_STARTED) return *this;
    rx_addr = _rx_addr;
    return *this;
}

wifi_mac &wifi_mac::set_transmitter_addr(const std::array<uint8_t, 6> &_tx_addr)
{
    // Ignore setting request if the action frame sender thread has started
    if((xEventGroupGetBits(wifi_mac_event) & WIFI_MAC_ACTION_STARTED) == WIFI_MAC_ACTION_STARTED) return *this;
    tx_addr = _tx_addr;
    return *this;
}

wifi_mac &wifi_mac::set_bssid_addr(const std::array<uint8_t, 6> &_bssid_addr)
{
    // Ignore setting request if the action frame sender thread has started
    if((xEventGroupGetBits(wifi_mac_event) & WIFI_MAC_ACTION_STARTED) == WIFI_MAC_ACTION_STARTED) return *this;
    bssid_addr = _bssid_addr;
    return *this;
}

wifi_mac &wifi_mac::set_oui(const std::array<uint8_t, 3> &_oui)
{
    // Ignore setting request if the action frame sender thread has started
    if((xEventGroupGetBits(wifi_mac_event) & WIFI_MAC_ACTION_STARTED) == WIFI_MAC_ACTION_STARTED) return *this;
    oui = _oui;
    return *this;
}

wifi_mac &wifi_mac::set_action_payload(const std::vector<uint8_t> &payload)
{
    // Ignore setting request if the action frame sender thread has started
    if((xEventGroupGetBits(wifi_mac_event) & WIFI_MAC_ACTION_STARTED) == WIFI_MAC_ACTION_STARTED) return *this;
    action_payload = payload;
    return *this;
}

wifi_mac &wifi_mac::set_wifi_interface(esp_interface_t interface)
{
    // Ignore setting request if the action frame sender thread has started
    if((xEventGroupGetBits(wifi_mac_event) & WIFI_MAC_ACTION_STARTED) == WIFI_MAC_ACTION_STARTED) return *this;
    curr_interface = interface;
    return *this;
}

wifi_mac::~wifi_mac()
{
    xEventGroupClearBits(wifi_mac_event, WIFI_MAC_ACTION_STARTED);
}
