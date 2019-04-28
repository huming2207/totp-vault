#pragma once

#include <totp_view.hpp>

class totp_controller
{
    public:
        totp_controller();
        void run_totp();

    private:
        totp_view view;
};