/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_freertos_hooks.h>
#include <esp_log.h>
#include <lv_core/lv_style.h>

#include "otp.h"
#include "lvgl.h"
#include "lvgl_hal_st7789.h"

#define TAG "esp-lvgl"

static void lvgl_main_task(void *p)
{
    while(1) {
        vTaskDelay(1);
        lv_task_handler();
    }
}


static void IRAM_ATTR lv_tick_cb()
{
    lv_tick_inc(portTICK_RATE_MS);
}

void app_main()
{
    printf("Hello world!\n");

    ESP_LOGI(TAG, "Initialising LittlevGL");
    lv_init();

    ESP_LOGI(TAG, "Initialising ST7789");
    lvgl_st7789_init();

    ESP_LOGI(TAG, "Initialising LVGL driver");
    lv_disp_drv_t disp;
    lv_disp_drv_init(&disp);
    disp.disp_fill = lvgl_st7789_fill;
    disp.disp_flush = lvgl_st7789_flush;

    ESP_LOGI(TAG, "Registering LVGL driver");
    lv_disp_drv_register(&disp);


    ESP_LOGI(TAG, "Registering LVGL tick callback");
    esp_register_freertos_tick_hook(lv_tick_cb);

    ESP_LOGI(TAG, "Starting LVGL main task");
    xTaskCreate(lvgl_main_task, "lv_task", 4096, NULL, 0, NULL);

    static lv_style_t totp_code_style;
    lv_style_copy(&totp_code_style, &lv_style_plain);
    totp_code_style.text.font = &dejavu_mono_40;

    static lv_style_t totp_hint_style;
    lv_style_copy(&totp_hint_style, &lv_style_plain);
    totp_hint_style.text.font = &lv_font_dejavu_20;

    lv_obj_t * upper_hint = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(upper_hint, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_style(upper_hint, &totp_hint_style);
    lv_label_set_text(upper_hint, "Google");
    lv_obj_set_width(upper_hint, 240);
    lv_obj_align(upper_hint, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 15);

    lv_obj_t * upper_code = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(upper_code, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_style(upper_code, &totp_code_style);
    lv_label_set_text(upper_code, "114514");
    lv_obj_set_width(upper_code, 240);
    lv_obj_align(upper_code, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 40);

    lv_obj_t * middle_hint = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(middle_hint, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_style(middle_hint, &totp_hint_style);
    lv_label_set_text(middle_hint, "Microsoft");
    lv_obj_set_width(middle_hint, 240);
    lv_obj_align(middle_hint, NULL, LV_ALIGN_IN_LEFT_MID, 5, -20);

    lv_obj_t * middle_code = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(middle_code, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_style(middle_code, &totp_code_style);
    lv_label_set_text(middle_code, "123456");
    lv_obj_set_width(middle_code, 240);
    lv_obj_align(middle_code, NULL, LV_ALIGN_IN_LEFT_MID, 0, 20);

    lv_obj_t * lower_hint = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(lower_hint, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_style(lower_hint, &totp_hint_style);
    lv_label_set_text(lower_hint, "Cloudflare");
    lv_obj_set_width(lower_hint, 240);
    lv_obj_align(lower_hint, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 5, -40);

    lv_obj_t * lower_code = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(lower_code, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_style(lower_code, &totp_code_style);
    lv_label_set_text(lower_code, "000000");
    lv_obj_set_width(lower_code, 240);
    lv_obj_align(lower_code, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
}

