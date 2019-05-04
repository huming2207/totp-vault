#pragma once

#include <totp_view.hpp>

class totp_controller
{
    public:
        totp_controller();
        void run_totp();

    private:
        totp_view view;
        uint32_t generate_token(const std::vector<uint8_t> &key);
};