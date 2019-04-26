#include <lv_core/lv_style.h>
#include "fp_add_view.h"

lv_obj_t *view_render_add_progress()
{
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.line.width = 15;
    style.line.color = LV_COLOR_CYAN;
    style.body.border.color = LV_COLOR_GRAY;
    style.body.border.width = 15;
    style.body.padding.hor = 0;

    lv_obj_t *add_prog = lv_preload_create(lv_scr_act(), NULL);
    lv_obj_set_size(add_prog, 125, 125);
    lv_obj_align(add_prog, NULL, LV_ALIGN_CENTER, 0, 25);
    lv_preload_set_style(add_prog, LV_PRELOAD_STYLE_MAIN, &style);

    return add_prog;
}

lv_obj_t *view_render_add_label()
{
    lv_obj_t * title = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(title, "Adding finger");
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

    return title;
}