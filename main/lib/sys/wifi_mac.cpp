#include <cstring>
#include <algorithm>

#include <esp_log.h>

#include "wifi_mac.hpp"

#define TAG "wifi_mac"

using namespace sys;

wifi_mac::wifi_mac()
{
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

esp_err_t wifi_mac::send_action_frame(esp_interface_t interface,
                                      const std::vector<uint8_t> &payload, uint8_t count)
{
    def::mgmt_frame_header header;

    // Initialise action header
    header.frame_type = def::FRAME_CTRL_MGMT_ACTION;
    header.frame_flag = 0;
    header.duration = 0;
    header.seq_frag = 0; // Placeholder for now, the WiFi PHY will fill in the correct value by itself
    std::memmove(header.receiver_mac, rx_addr.data(), rx_addr.size());
    std::memmove(header.transmitter_mac, tx_addr.data(), tx_addr.size());
    std::memmove(header.bssid, bssid_addr.data(), bssid_addr.size());

    std::vector<uint8_t> frame;

    // Insert Action frame header
    frame.insert(frame.end(), header.val, header.val + sizeof(def::mgmt_frame_header));

    // Append Action frame type
    frame.emplace_back(0x7f); // Vendor-specific Action frame

    // Append OUI
    frame.insert(frame.end(),
                 std::make_move_iterator(oui.begin()),
                 std::make_move_iterator(oui.end()));

    // Append actual data payload
    frame.insert(frame.end(),
                 std::make_move_iterator(action_payload.begin()),
                 std::make_move_iterator(action_payload.end()));

    auto ret = ESP_OK;
    for(uint8_t idx = 0; idx < count; idx ++) {
        ret = ret ?: esp_wifi_80211_tx(interface, frame.data(), frame.size(), true);
        vTaskDelay(pdMS_TO_TICKS(150));
    }

    return ret;
}

wifi_mac &wifi_mac::set_receiver_addr(const std::array<uint8_t, 6> &_rx_addr)
{
    // Ignore setting request if the action frame sender thread has started
    rx_addr = _rx_addr;
    return *this;
}

wifi_mac &wifi_mac::set_transmitter_addr(const std::array<uint8_t, 6> &_tx_addr)
{
    tx_addr = _tx_addr;
    return *this;
}

wifi_mac &wifi_mac::set_bssid_addr(const std::array<uint8_t, 6> &_bssid_addr)
{
    bssid_addr = _bssid_addr;
    return *this;
}

wifi_mac &wifi_mac::set_oui(const std::array<uint8_t, 3> &_oui)
{
    oui = _oui;
    return *this;
}

wifi_mac &wifi_mac::set_action_payload(const std::vector<uint8_t> &payload)
{
    action_payload = payload;
    return *this;
}

wifi_mac &wifi_mac::set_wifi_interface(esp_interface_t interface)
{
    curr_interface = interface;
    return *this;
}

