#pragma once

#include <string>
#include <nvs_flash.h>
#include <vector>
#include <key_uri.hpp>

class token_vector : private std::vector<key_uri>
{
    public:
        token_vector();
        esp_err_t save();
    private:
        nvs_handle_t handle = 0;
};

