#pragma once

#include "view.hpp"

class fp_add_view : public view
{
    public:
        fp_add_view();
        lv_obj_t *get_object(const std::string& name) override;

    private:
        static lvgl_obj_ptr view_render_add_progress();
        static lvgl_obj_ptr view_render_add_label();
        lvgl_obj_map obj_map{};
};
