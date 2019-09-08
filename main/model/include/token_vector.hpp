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
        using vector::at;
        using vector::shrink_to_fit;
        using vector::begin;
        using vector::end;
        static token_vector& instance()
        {
            static token_vector vec;
            return vec;
        };
        token_vector(token_vector const&) = delete;
        void operator=(token_vector const&) = delete;
        esp_err_t save();
    private:
        token_vector();
        nvs_handle_t handle = 0;
};

