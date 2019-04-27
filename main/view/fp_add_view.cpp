#include <lv_core/lv_style.h>
#include "fp_add_view.h"

lv_obj_t *view_render_add_progress()
{
    lv_obj_t * progress = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_size(progress, 200, 50);
    lv_obj_align(progress, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_value(progress, 0);

    return progress;
}

lv_obj_t *view_render_add_label()
{
    lv_obj_t * title = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(title, "Adding finger");
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

    return title;
}