#include "fp_add_view.hpp"

fp_add_view::fp_add_view()
{
    obj_map.emplace("add_progress", view_render_add_progress());
    obj_map.emplace("add_label", view_render_add_label());
}

std::unique_ptr<lv_obj_t> fp_add_view::view_render_add_progress()
{
    lv_obj_t * progress = lv_bar_create(lv_scr_act(), nullptr);
    lv_obj_set_size(progress, 200, 50);
    lv_obj_align(progress, nullptr, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_value(progress, 0);

    return std::unique_ptr<lv_obj_t>(progress);
}

std::unique_ptr<lv_obj_t> fp_add_view::view_render_add_label()
{
    lv_obj_t * title = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text(title, "Adding finger");
    lv_obj_align(title, nullptr, LV_ALIGN_IN_TOP_MID, 0, 10);

    return std::unique_ptr<lv_obj_t>(title);
}

lv_obj_t *fp_add_view::get_object(const std::string& name)
{
    return obj_map[name].get();
}


