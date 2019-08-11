#pragma once

#include <array>
#include <vector>
#include <cstdint>

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_wifi.h>
#include <esp_err.h>

#define WIFI_MAC_BROADCAST_ADDR { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }
#define WIFI_MAC_DEFAULT_OUI    { 0x18, 0xfe, 0x34 }

#define WIFI_MAC_ACTION_STARTED BIT0

namespace sys
{
    namespace def
    {
        enum device_config_state {
            DEVICE_CONFIGURED       = 0,
            DEVICE_INIT_BRAND_NEW   = 1,
            DEVICE_INIT_RESET       = 2
        };

        enum wifi_frame_type {
            FRAME_CTRL_MGMT_ACTION                   = 0xD0,
            FRAME_CTRL_MGMT_BEACON                   = 0x80,
            FRAME_CTRL_MGMT_PROBE_RESPONSE           = 0x50,
            FRAME_CTRL_MGMT_PROBE_REQUEST            = 0x40,
            FRAME_CTRL_MGMT_REASSOC_REQUEST          = 0x20,
            FRAME_CTRL_MGMT_REASSOC_RESPONSE         = 0x30,
            FRAME_CTRL_MGMT_ASSOCIATE_REQUEST        = 0x00,
            FRAME_CTRL_MGMT_ASSOCIATE_RESPONSE       = 0x10
        };

        typedef struct __attribute__((packed)) {
            uint8_t element_id:8;      /**< Should be set to WIFI_VENDOR_IE_ELEMENT_ID (0xDD) */
            uint8_t length:8;          /**< Length of all bytes in the element data following this field. Minimum 4. */
            uint8_t vendor_oui[3];   /**< Vendor identifier (OUI). */
            uint8_t vendor_oui_type:8; /**< Vendor-specific OUI type. */
            uint8_t mac[6];
            uint8_t idf_ver[32];
            device_config_state state:8;
        } smoke_signal;

        typedef struct __attribute__((packed)) {
            union __attribute__((packed)) {
                struct __attribute__((packed)) {
                    wifi_frame_type frame_type:8;
                    uint8_t frame_flag;
                    uint16_t duration:16;
                    uint8_t receiver_mac[6];
                    uint8_t transmitter_mac[6];
                    uint8_t bssid[6];
                    uint16_t seq_frag:16;
                };
                uint8_t val[24];
            };
        } mgmt_frame_header;
    }

    class wifi_mac
    {
        // Hidden default constructor
        private:
            wifi_mac();
            ~wifi_mac();

        // Private members
        private:
            std::array<uint8_t, 6> rx_addr = WIFI_MAC_BROADCAST_ADDR;
            std::array<uint8_t, 6> tx_addr{};
            std::array<uint8_t, 6> bssid_addr = WIFI_MAC_BROADCAST_ADDR;
            std::array<uint8_t, 3> oui = WIFI_MAC_DEFAULT_OUI;
            esp_interface_t curr_interface = ESP_IF_WIFI_AP;
            std::vector<uint8_t> action_payload;
            static EventGroupHandle_t wifi_mac_event;

        // Singleton related
        public:
            wifi_mac(wifi_mac const&) = delete;
            void operator=(wifi_mac const&) = delete;
            static wifi_mac& get_instance();

        // Control APIs
        public:
            esp_err_t set_smoke_signal(def::device_config_state state);
            esp_err_t unset_smoke_signal();
            wifi_mac& start_send_action_frame(esp_interface_t interface, const std::vector<uint8_t> &payload);
            wifi_mac& stop_send_action_frame();

        // Setter & builders
        public:
            wifi_mac& set_receiver_addr(const std::array<uint8_t, 6>& _rx_addr);
            wifi_mac& set_transmitter_addr(const std::array<uint8_t, 6>& _tx_addr);
            wifi_mac& set_bssid_addr(const std::array<uint8_t, 6>& _bssid_addr);
            wifi_mac& set_oui(const std::array<uint8_t, 3>& _oui);
            wifi_mac& set_action_payload(const std::vector<uint8_t>& payload);
            wifi_mac& set_wifi_interface(esp_interface_t interface);
    };
}

