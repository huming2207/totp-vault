#include "config_view.hpp"

config_view::config_view(const std::string& ssid)
{
    main_label = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_long_mode(main_label, LV_LABEL_LONG_BREAK);
    lv_label_set_align(main_label, LV_LABEL_ALIGN_LEFT);
    lv_label_set_recolor(main_label, true);
    lv_obj_set_width(main_label, 130);
    lv_obj_align(main_label, nullptr, LV_ALIGN_CENTER, 0, -20);

    std::string label_txt = "Please connect to\nWiFi AP:\n#ff0000 " + ssid + '#';
    lv_label_set_text(main_label, label_txt.c_str());
}

config_view::~config_view()
{
    lv_obj_del(main_label);
}
