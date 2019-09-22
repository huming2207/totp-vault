#include <esp_system.h>
#include "config_ctrl.hpp"

config_ctrl::config_ctrl()
{
    uint8_t mac[6] = { 0 };
    char mac_lsb_str[7] = { 0 };
    esp_efuse_mac_get_default(mac);
    sprintf(mac_lsb_str, "%02x%02x%02x", mac[3], mac[4], mac[5]);
    wifi_ssid += mac_lsb_str;
}
