#include "fp_add_view.hpp"

fp_add_view::fp_add_view() : mapped_view()
{
    obj_map.emplace("add_progress", view_render_progress_bar());
    obj_map.emplace("add_label", view_render_progress_label());
}

lvgl_obj_ptr fp_add_view::view_render_progress_bar()
{
    lv_obj_t * progress = lv_bar_create(lv_scr_act(), nullptr);
    lv_obj_set_size(progress, 150, 50);
    lv_obj_align(progress, nullptr, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_value(progress, 0, LV_ANIM_ON);

    return view::make_object(progress);
}

lvgl_obj_ptr fp_add_view::view_render_progress_label()
{
    lv_obj_t * title = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text(title, "Adding finger");
    lv_obj_set_width(title, 240);
    lv_label_set_long_mode(title, LV_LABEL_LONG_BREAK);
    lv_obj_align(title, nullptr, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);

    return view::make_object(title);
}

void fp_add_view::set_title_label_text(const std::string &str)
{
    lv_label_set_text(get_object("add_label"), str.c_str());
}

lv_obj_t *fp_add_view::get_object(const std::string& name)
{
    return obj_map[name].get();
}


