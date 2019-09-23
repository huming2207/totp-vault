#pragma once

#include <sntp_view.hpp>

class sntp_ctrl
{
    public:
        sntp_ctrl();
        void connect_ap();
        void sync_time();

    private:
        sntp_view view;
};
