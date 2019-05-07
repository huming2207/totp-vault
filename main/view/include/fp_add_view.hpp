#pragma once

#include "mapped_view.hpp"

class fp_add_view : public mapped_view
{
    public:
        fp_add_view();
        lv_obj_t *get_object(const std::string& name);
        void set_title_label_text(const std::string &str);

    private:
        static lvgl_obj_ptr view_render_progress_bar();
        static lvgl_obj_ptr view_render_progress_label();
};
