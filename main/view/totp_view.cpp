#include "totp_view.hpp"

totp_view::totp_view()
{
    view_render_totp_labels(5, 15, 0, 40, "upper_title", "upper_code", LV_ALIGN_IN_TOP_LEFT);
    view_render_totp_labels(5, -15, 0, 15, "mid_title", "mid_code", LV_ALIGN_IN_LEFT_MID);
    view_render_totp_labels(5, -45, 0, -5, "lower_title", "lower_code", LV_ALIGN_IN_BOTTOM_LEFT);
}

lv_obj_t *totp_view::get_object(const std::string &name)
{
    return obj_map[name].get();
}

void totp_view::view_render_totp_labels(int16_t title_x, int16_t title_y, int16_t code_x, int16_t code_y,
                                        const std::string& title_name, const std::string& code_name, lv_align_t align)
{
    static lv_style_t totp_title_style;
    lv_style_copy(&totp_title_style, &lv_style_plain);
    totp_title_style.text.font = &lv_font_dejavu_20;

    static lv_style_t totp_code_style;
    lv_style_copy(&totp_code_style, &lv_style_plain);
    totp_code_style.text.font = &dejavu_mono_40;

    lv_obj_t * title = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_align(title, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_style(title, &totp_title_style);
    lv_label_set_text(title, "Unknown");
    lv_obj_set_width(title, 240);
    lv_obj_align(title, nullptr, align, title_x, title_y);

    obj_map.emplace(title_name, view::make_object(title));

    lv_obj_t * code = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_align(code, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_style(code, &totp_code_style);
    lv_label_set_text(code, "000000");
    lv_obj_set_width(code, 240);
    lv_obj_align(code, nullptr, align, code_x, code_y);

    obj_map.emplace(code_name, view::make_object(code));
}

lvgl_obj_map &totp_view::get_obj_map()
{
    return obj_map;
}


