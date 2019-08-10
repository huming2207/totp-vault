#pragma once

#include <string>
#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_wifi.h>
#include <array>

#define WIFI_MG_BROADCAST_ADDR { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }
#define WIFI_MG_DEFAULT_OUI    { 0x18, 0xfe, 0x34 }

namespace sys
{
    namespace def
    {
        enum wifi_event_bit {
            WIFI_STA_CONNECTED                  = BIT0,
            WIFI_STA_CONNECTED_IPV6             = BIT1,
            WIFI_AP_DEVICE_CONNECT              = BIT2,
            WIFI_AP_DEVICE_DISCONNECT           = BIT3,
            WIFI_STA_CONNECT_RETRY_MAXIMUM      = BIT4
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

        enum device_config_state {
            DEVICE_CONFIGURED       = 0,
            DEVICE_INIT_BRAND_NEW   = 1,
            DEVICE_INIT_RESET       = 2
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

    class wifi_manager
    {
        public:
            wifi_manager(wifi_manager const&) = delete;
            void operator=(wifi_manager const&) = delete;
            static wifi_manager& get_manager();
            esp_err_t set_ap_config(const std::string &ssid, const std::string &passwd, uint8_t channel);
            esp_err_t set_sta_config(const std::string &ssid, const std::string &passwd, bool fast_scan = false);
            esp_err_t start(wifi_mode_t mode = WIFI_MODE_AP);
            esp_err_t stop();
            esp_err_t enable_smoke_signal();
            esp_err_t recv_smoke_signal(time_t timeout);
            esp_err_t send_action_frame(esp_interface_t interface, const std::vector<uint8_t>& payload,
                                        const std::array<uint8_t, 6>& recv_addr = WIFI_MG_BROADCAST_ADDR,
                                        const std::array<uint8_t, 6>& bssid = WIFI_MG_BROADCAST_ADDR,
                                        const std::array<uint8_t, 3>& oui = WIFI_MG_DEFAULT_OUI, uint8_t repeat = 5);
            esp_err_t get_ap_config(wifi_config_t &config);
            esp_err_t get_sta_config(wifi_config_t &config);

        private:
            wifi_manager();
            static uint8_t sta_retry_cnt;
            static uint8_t sta_max_retry;
            static EventGroupHandle_t wifi_event_group;
            static ip6_addr_t ip6_addr;
            static ip_addr_t ip_addr;
    };
}