#pragma once

#include <tuple>
#include "view.hpp"

typedef std::tuple<lvgl_obj_ptr, lvgl_obj_ptr> totp_label_obj;

class totp_view : public view
{
    public:
        totp_view();
        lv_obj_t *get_object(const std::string& name) override;
        lvgl_obj_map &get_obj_map();

    private:
        void view_render_totp_labels(int16_t title_x, int16_t title_y, int16_t code_x, int16_t code_y,
                                     const std::string& title_name, const std::string& code_name, lv_align_t align);
        lvgl_obj_map obj_map{};
};

