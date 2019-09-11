#include <cstring>

#include "token_vector.hpp"
#include <nvs.h>
#include <esp_log.h>

#define TAG "token_model"

token_vector::token_vector()
{
    nvs_flash_init();
    ESP_ERROR_CHECK(nvs_open("token", NVS_READWRITE, &handle));
    auto uri_it = nvs_entry_find("nvs", "token", NVS_TYPE_STR);
    if(uri_it == nullptr) {
        ESP_LOGW(TAG, "Nothing found from the token storage!");
        return;
    }

    char uri_str[256] = { 0 };
    nvs_entry_info_t info{};
    size_t uri_len = 0;

    while (uri_it != nullptr) {
        std::memset(uri_str, '\0', sizeof(uri_str));
        std::memset(&info, 0, sizeof(nvs_entry_info_t));
        nvs_entry_info(uri_it, &info);
        uri_it = nvs_entry_next(uri_it);
        if(nvs_get_str(handle, info.key, uri_str, &uri_len) == ESP_OK) {
            emplace_back(otp_key(uri_str));
        }

        ESP_LOGI(TAG, "Got token: %s", info.key);
    }
}

esp_err_t token_vector::save()
{
    // Clear up the stuff in token namespace first
    auto ret = nvs_erase_all(handle);

    for(size_t idx = 0; idx < size(); idx++) {
        ret = ret ?: nvs_set_str(handle, std::to_string(idx).c_str(), at(idx).get_uri().c_str());
    }

    return ret;
}

