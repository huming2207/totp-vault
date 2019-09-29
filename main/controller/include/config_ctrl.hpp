#pragma once

#include <m5stickc.hpp>
#include <string>
#include "ictrl.hpp"

class config_ctrl : protected ictrl
{
    public:
        config_ctrl();

    private:
        std::string wifi_ssid = "vault-";
};

