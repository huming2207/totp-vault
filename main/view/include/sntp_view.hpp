#pragma once

#include <lvgl.h>

class sntp_view
{
    public:
        sntp_view();
        ~sntp_view();

        void set_sync_text();
        void set_connect_text();
        void set_done_text();
        void set_wifi_error_text();
        void set_sync_error_text();

    private:

    private:
        lv_obj_t* status_label = nullptr;
        lv_obj_t* icon_label = nullptr;
};
