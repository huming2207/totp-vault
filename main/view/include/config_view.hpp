#pragma once

#include <lvgl.h>
#include <string>

class config_view
{
    public:
        config_view(const std::string& ssid);
        ~config_view();

    private:
        lv_obj_t* main_label = nullptr;
};

