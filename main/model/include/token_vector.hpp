#pragma once

#include <string>
#include <nvs_flash.h>
#include <vector>
#include <otp_key.hpp>

class token_vector : private std::vector<otp_key>
{
    public:
        using vector::emplace_back;
        using vector::operator[];
        using vector::begin;
        using vector::end;
        token_vector();
        esp_err_t save();
    private:
        nvs_handle_t handle = 0;
};

