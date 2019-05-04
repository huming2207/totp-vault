#pragma once

#include <tuple>
#include <vector>
#include "view.hpp"

typedef std::tuple<lvgl_obj_ptr, lvgl_obj_ptr> totp_label_tuple;
typedef std::vector<totp_label_tuple>  totp_obj_list;

class totp_view : public view
{
    public:
        totp_view();
        totp_obj_list &get_obj_list();
        void set_label(uint8_t target, const char *title, const char *token);

    private:
        void view_render_totp_labels(int16_t title_x, int16_t title_y, int16_t code_x, int16_t code_y, lv_align_t align);
        totp_obj_list obj_list{};
};

