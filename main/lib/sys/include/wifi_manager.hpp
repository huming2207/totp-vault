#pragma once

#include <string>
#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_wifi.h>

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