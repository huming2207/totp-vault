#pragma once

#include <string>
#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_wifi.h>
#include <array>
#include <functional>

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
            esp_err_t get_sta_config(wifi_config_t &config);
            esp_err_t get_ap_config(wifi_config_t &config);
            void on_connected(const std::function<void(const std::array<uint8_t, 6> mac, uint8_t aid)>& cb);
            void on_disconnected(const std::function<void(const std::array<uint8_t, 6> mac, uint8_t aid)>& cb);
            void on_sta_disconnect(const std::function<void()>& cb);
            void on_sta_connect_lost(const std::function<void()>& cb);
            void on_dhcp_done(const std::function<void()>& cb);
            void on_dhcp6_done(const std::function<void()>& cb);

        private:
            wifi_manager();
            std::function<void(const std::array<uint8_t, 6> mac, uint8_t aid)> connected_cb;
            std::function<void(const std::array<uint8_t, 6> mac, uint8_t aid)> disconnected_cb;
            std::function<void()> sta_disconnect_cb;
            std::function<void()> sta_connect_lost_cb;
            std::function<void()> dhcp_done_cb;
            std::function<void()> dhcp6_done_cb;

            static void wifi_evt_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
            static void ip_evt_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
            static uint8_t sta_retry_cnt;
            static uint8_t sta_max_retry;
            static EventGroupHandle_t wifi_event_group;
            static ip6_addr_t ip6_addr;
            static ip_addr_t ip_addr;
    };
}