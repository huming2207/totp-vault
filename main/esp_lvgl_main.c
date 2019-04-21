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

    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 100, 50);                          /*Set its size*/

    lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, NULL);/*Assign a callback to the button*/
    lv_obj_t * label = lv_label_create(btn, NULL);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");                     /*Set the labels text*/

    lv_obj_t * txt = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(txt, LV_LABEL_LONG_BREAK);     /*Break the long lines*/
    lv_label_set_recolor(txt, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_align(txt, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
    lv_label_set_text(txt, "This is a ST7789V IPS module\n"
                           "Together with ESP32\n"
                           "Running LittlevGL\n"
                           "Ported by Jackson Hu\n");
    lv_obj_set_width(txt, 240);                           /*Set a width*/
    lv_obj_align(txt, NULL, LV_ALIGN_CENTER, 0, 20);      /*Align to center*/
}

