#pragma once

#include <lvgl.h>
#include <string>

class totp_view
{
    public:
        totp_view();
        ~totp_view();

        void set_token(const std::string& token, const std::string& remark);

    private:
        lv_obj_t* token_label = nullptr;
        lv_obj_t* remark_label = nullptr;
        static lv_style_t token_style;
};
