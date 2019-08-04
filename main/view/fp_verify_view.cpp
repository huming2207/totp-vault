#include "fp_verify_view.hpp"


fp_verify_view::fp_verify_view()
{
    obj_map.emplace("title", view_render_title());
    obj_map.emplace("icon", view_render_icon());
}

lvgl_obj_ptr fp_verify_view::view_render_title()
{
    lv_obj_t * title = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text(title, "Verify your finger");
    lv_obj_set_width(title, 240);
    lv_label_set_long_mode(title, LV_LABEL_LONG_BREAK);
    lv_obj_align(title, nullptr, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);

    return view::make_object(title);
}

lvgl_obj_ptr fp_verify_view::view_render_icon()
{
    static lv_style_t style_txt;
    lv_style_copy(&style_txt, &lv_style_plain);

    lv_obj_t * icon = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text(icon, LV_SYMBOL_REFRESH);
    lv_obj_set_width(icon, 240);
    lv_obj_set_style(icon, &style_txt);
    lv_obj_align(icon, nullptr, LV_ALIGN_IN_LEFT_MID, 85, 0);
    lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);

    return view::make_object(icon);
}

void fp_verify_view::set_icon(const char *icon)
{
    lv_label_set_text(obj_map["icon"].get(), icon);
}

void fp_verify_view::set_title_text(const char *text)
{
    lv_label_set_text(obj_map["title"].get(), text);
}
