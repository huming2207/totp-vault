/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <cstdio>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_freertos_hooks.h>
#include <esp_log.h>
#include <lv_core/lv_style.h>

#include "lvgl.h"
#include "lvgl_hal_st7789.h"

#include <wifi_manager.hpp>
#include <rest_controller.hpp>

#include <esp_now.h>
#include "lvgl_hal_st7735r.h"

#define TAG "esp-lvgl"

extern "C" {
    void app_main();
}


static void lvgl_main_task(void *p)
{
    while(true) {
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

//    ESP_LOGI(TAG, "Initialising NVS");
//    nv_storage::flash_init();

    ESP_LOGI(TAG, "Initialising LittlevGL");
    lv_init();

    ESP_LOGI(TAG, "Initialising ST7789");
    lvgl_st7735r_init();

    ESP_LOGI(TAG, "Initialising LVGL driver");
    static lv_color_t buf1[ST7735R_BUF_SIZE];
    static lv_color_t buf2[ST7735R_BUF_SIZE];
    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, buf1, buf2, ST7735R_BUF_SIZE);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = lvgl_st7735r_flush_cb;

    ESP_LOGI(TAG, "Registering LVGL driver");
    lv_disp_drv_register(&disp_drv);


    ESP_LOGI(TAG, "Registering LVGL tick callback");
    esp_register_freertos_tick_hook(lv_tick_cb);

    ESP_LOGI(TAG, "Starting LVGL main task");
    xTaskCreate(lvgl_main_task, "lv_task", 8192, nullptr, 5, nullptr);

    vTaskDelay(portMAX_DELAY);
}

