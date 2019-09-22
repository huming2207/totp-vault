//
// Created by hu on 15/9/19.
//

#include "sntp_view.hpp"

sntp_view::sntp_view()
{
    status_label = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_long_mode(status_label, LV_LABEL_LONG_BREAK);
    lv_label_set_text(status_label, "Unknown status");
    lv_obj_set_width(status_label, 125);
    lv_obj_align(status_label, nullptr, LV_ALIGN_CENTER, 15, -10);

    icon_label = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text(icon_label, LV_SYMBOL_WARNING);
    lv_obj_align(icon_label, nullptr, LV_ALIGN_IN_LEFT_MID, 10, 0);
}

void sntp_view::set_connect_text()
{
    lv_label_set_text(status_label, "Waiting for\nWiFi connection");
    lv_label_set_text(icon_label, LV_SYMBOL_WIFI);
}

void sntp_view::set_sync_text()
{
    lv_label_set_text(status_label, "Syncing time\nfrom network...");
    lv_label_set_text(icon_label, LV_SYMBOL_REFRESH);
}


void sntp_view::set_done_text()
{
    lv_label_set_text(status_label, "Done!\nreboot in 5 sec...");
    lv_label_set_text(icon_label, LV_SYMBOL_OK);
}

void sntp_view::set_wifi_error_text()
{
    lv_label_set_text(status_label, "Failed!\nNo WiFi AP");
    lv_label_set_text(icon_label, LV_SYMBOL_CLOSE);
}

void sntp_view::set_sync_error_text()
{
    lv_label_set_text(status_label, "Failed!\nSync failed");
    lv_label_set_text(icon_label, LV_SYMBOL_CLOSE);
}

sntp_view::~sntp_view()
{
    lv_obj_del(status_label);
    lv_obj_del(icon_label);
}
