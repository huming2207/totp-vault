#pragma once

#include "mapped_view.hpp"

class fp_verify_view : public mapped_view
{
    public:
        fp_verify_view();
        void set_icon(const char *icon);
        void set_title_text(const char *text);

    private:
        lvgl_obj_ptr view_render_title();
        lvgl_obj_ptr view_render_icon();

        lvgl_obj_map obj_map{};
};