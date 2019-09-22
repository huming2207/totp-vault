#include "totp_view.hpp"

lv_style_t totp_view::token_style{};

totp_view::totp_view()
{
    lv_style_copy(&token_style, &lv_style_plain);
    token_style.text.font = &dejavu_mono_40;

    token_label = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_style(token_label, LV_LABEL_STYLE_MAIN, &token_style);
    lv_label_set_text(token_label, "123456");
    lv_obj_set_width(token_label, 135);
    lv_obj_align(token_label, nullptr, LV_ALIGN_CENTER, 0, -10);

    remark_label = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_long_mode(remark_label, LV_LABEL_LONG_SROLL_CIRC);
    lv_label_set_text(remark_label, "Unknown account (unknown@unknown.com)");
    lv_obj_set_width(remark_label, 135);
    lv_obj_align(remark_label, nullptr, LV_ALIGN_CENTER, 0, 20);
}

totp_view::~totp_view()
{
    lv_obj_del(token_label);
    lv_obj_del(remark_label);
}

void totp_view::set_token(const std::string &token, const std::string &remark)
{
    lv_label_set_text(token_label, token.c_str());
    lv_label_set_text(remark_label, remark.c_str());
}
