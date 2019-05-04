#include <base32.hpp>
#include "totp_model.hpp"

#define TAG "totp_model"
#define TOTP_LIST_KEY       "totp_list"
#define TOTP_COUNT_KEY      "totp_count"

totp_model::totp_model() : nvs("vault", NVS_READWRITE)
{
    refresh_list();
}

esp_err_t totp_model::add_totp_tuple(const std::string &title, const std::string &token)
{
    // Decode Base32 token to byte array, following Google's method
    // Ref: https://github.com/google/google-authenticator-libpam/blob/master/src/google-authenticator.c#L59
    size_t token_len = (token.length() + 7) / 8 * BITS_PER_BASE32_CHAR;
    auto *token_decoded = new uint8_t[token_len];
    if((token_len = base32::decode(token.c_str(), token_decoded, token_len)) < 1) return ESP_ERR_INVALID_SIZE;

    totp_list.push_back({ title, std::vector<uint8_t>(token_decoded, token_decoded + token_len) });

    delete[] token_decoded; // Delete decoded buffer after use...
    save_list();
    return ESP_OK;
}

esp_err_t totp_model::remove_totp_tuple(const std::string &title)
{
    for(auto it = totp_list.begin(); it != totp_list.end(); ++it) {
        auto curr_tuple = *it;
        if(title == curr_tuple.title) {
            totp_list.erase(it); // Remove current tuple
            save_list();
            return ESP_OK;
        }
    }

    return ESP_ERR_NOT_FOUND;
}

void totp_model::refresh_list()
{
    uint8_t count = 0;
    esp_err_t ret = nvs.read(TOTP_COUNT_KEY, &count);
    if(ret == ESP_ERR_NVS_NOT_INITIALIZED) {
        ESP_ERROR_CHECK(nv_storage::flash_init());
        ESP_ERROR_CHECK(nvs.read(TOTP_LIST_KEY, &count));
    } else if(ret == ESP_ERR_NVS_NOT_FOUND) {
        ESP_ERROR_CHECK(nvs.write(TOTP_COUNT_KEY, count));
    }

    if(count < 1) return; // Forget about reading TOTP array blob anyway...

    totp_list.clear();
    auto *token_list_array = new totp_tuple_t[count];
    totp_list = std::vector<totp_tuple_t>(token_list_array, token_list_array + count);
    delete[] token_list_array;
}

void totp_model::save_list()
{
    ESP_ERROR_CHECK(nvs.write(TOTP_LIST_KEY, totp_list.data()));
    ESP_ERROR_CHECK(nvs.write(TOTP_LIST_KEY, totp_list.size()));
}

totp_tuple_t totp_model::get_totp_tuple(const std::string &title)
{
    for(auto curr_tuple : totp_list) {
        if(title == curr_tuple.title) return curr_tuple;
    }
}

totp_tuple_list totp_model::get_totp_list()
{
    return totp_list;
}

totp_model::~totp_model()
{
    save_list();
    totp_list.clear();
}
