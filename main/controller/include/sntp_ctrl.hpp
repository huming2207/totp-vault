#pragma once

#include <sntp_view.hpp>
#include "ictrl.hpp"

class sntp_ctrl : protected ictrl
{
    public:
        sntp_ctrl();

    private:
        void connect_ap();
        void sync_time();
        sntp_view view;

};
