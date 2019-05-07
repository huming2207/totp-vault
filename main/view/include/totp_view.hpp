#pragma once

#include <vector>
#include "view.hpp"
#include "listed_view.hpp"


class totp_view : public listed_view
{
    public:
        totp_view();
        lvgl_obj_list &get_obj_list();
        void set_label(uint8_t target, const char *title, const char *token);

    private:
        void view_render_totp_labels(int16_t title_x, int16_t title_y, int16_t code_x, int16_t code_y, lv_align_t align);
};

