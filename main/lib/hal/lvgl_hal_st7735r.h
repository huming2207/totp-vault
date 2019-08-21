#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <lvgl.h>

#define ST7735R_BUF_SIZE (LV_HOR_RES_MAX * 20)

void lvgl_st7735r_init();
void lvgl_st7735r_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color);



#ifdef __cplusplus
}
#endif