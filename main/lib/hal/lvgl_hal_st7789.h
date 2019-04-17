#pragma once

#define ST7789_CMD 0
#define ST7789_DAT 1

typedef struct {
    uint8_t reg;
    uint8_t data[6];
    uint8_t len;
} st7789_seq_t;

void lvgl_st7789_init();
void lvgl_st7789_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);