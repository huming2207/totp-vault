#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ST7789_CMD 0
#define ST7789_DAT 1

typedef struct {
    uint8_t reg;
    uint8_t data[6];
    uint8_t len;
} st7789_seq_t;

void lvgl_st7789_init();
void lvgl_st7789_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
void lvgl_st7789_flush(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

#ifdef __cplusplus
}
#endif