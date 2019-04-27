#pragma once

#include "view.hpp"


class fp_add_view : public view
{
    public:
        fp_add_view();
        lv_obj_t *get_object(const std::string& name);

    private:
        std::unique_ptr<lv_obj_t> view_render_add_progress();
        std::unique_ptr<lv_obj_t> view_render_add_label();
        lvgl_obj_map obj_map{};
};
