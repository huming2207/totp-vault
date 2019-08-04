#include "totp_view.hpp"

totp_view::totp_view() : listed_view()
{
    view_render_totp_labels(5, 15, 0, 40, LV_ALIGN_IN_TOP_LEFT);
    view_render_totp_labels(5, -15, 0, 15, LV_ALIGN_IN_LEFT_MID);
    view_render_totp_labels(5, -45, 0, -5, LV_ALIGN_IN_BOTTOM_LEFT);
}

void totp_view::view_render_totp_labels(int16_t title_x, int16_t title_y, int16_t code_x, int16_t code_y,
                                        lv_align_t align)
{
    static lv_style_t totp_title_style;
    lv_style_copy(&totp_title_style, &lv_style_plain);
    totp_title_style.text.font = &lv_font_roboto_16;

    static lv_style_t totp_code_style;
    lv_style_copy(&totp_code_style, &lv_style_plain);
    totp_code_style.text.font = &dejavu_mono_40;

    lv_obj_t * title = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_align(title, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_style(title, &totp_title_style);
    lv_label_set_text(title, "Unknown");
    lv_obj_set_width(title, 240);
    lv_obj_align(title, nullptr, align, title_x, title_y);

    lv_obj_t * code = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_align(code, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_style(code, &totp_code_style);
    lv_label_set_text(code, "000000");
    lv_obj_set_width(code, 240);
    lv_obj_align(code, nullptr, align, code_x, code_y);

    obj_list.emplace_back(make_obj_tuple(view::make_object(title), view::make_object(code)));
}

lvgl_obj_list &totp_view::get_obj_list()
{
    return obj_list;
}

void totp_view::set_label(uint8_t target, const char *title, const char *token)
{
    auto &tuple = obj_list[target];
    auto &title_label = tuple[0];
    auto &token_label = tuple[1];

    lv_label_set_text(title_label.get(), title);
    lv_label_set_text(token_label.get(), token);
}

