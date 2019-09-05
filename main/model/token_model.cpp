#include "token_model.hpp"

token_model::token_model()
{
    ESP_ERROR_CHECK(nvs_open("totp", NVS_READWRITE, &handle));
}

size_t token_model::count()
{
    size_t entries = 0;
    nvs_get_used_entry_count(handle, &entries);
    return entries;
}

