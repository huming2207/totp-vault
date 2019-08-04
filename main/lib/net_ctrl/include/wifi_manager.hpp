#pragma once

#include <string>
#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_wifi.h>

namespace net_ctrl
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

        typedef struct __attribute__((packed)) {
            uint8_t ssid[32];
            uint8_t passwd[64];
        } cheers_payload;
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
            esp_err_t set_cheers_payload();
            esp_err_t set_cheers_payload(std::vector<uint8_t> payload);
            esp_err_t recv_cheers(time_t timeout);

        private:
            wifi_manager();
            static uint8_t sta_retry_cnt;
            static uint8_t sta_max_retry;
            static EventGroupHandle_t wifi_event_group;
            static ip6_addr_t ip6_addr;
            static ip_addr_t ip_addr;
    };
}