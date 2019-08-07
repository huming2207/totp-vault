/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_freertos_hooks.h>
#include <esp_log.h>
#include <lv_core/lv_style.h>
#include <totp_controller.hpp>

#include "otp.h"
#include "fp1020a.h"
#include "lvgl.h"
#include "lvgl_hal_st7789.h"

#include "fp_add_controller.hpp"
#include "fp_verify_controller.hpp"

#include <wifi_manager.hpp>
#include <rest_controller.hpp>

#define TAG "esp-lvgl"

extern "C" {
    void app_main();
}


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

    auto &wifi_mgr = net_ctrl::wifi_manager::get_manager();
    ESP_ERROR_CHECK(wifi_mgr.set_sta_config("hu-home", "***huhome6789"));
    ESP_ERROR_CHECK(wifi_mgr.set_ap_config("Yeet", "test123456", 3));
    ESP_ERROR_CHECK(wifi_mgr.start(WIFI_MODE_APSTA));
    auto rest_mgr = web_ctrl::rest_controller();
    vTaskDelay(portMAX_DELAY);

////    ESP_LOGI(TAG, "Initialising NVS");
////    nv_storage::flash_init();
//
//    ESP_LOGI(TAG, "Initialising FP1020");
//    fp1020a_init();
//    fp1020a_remove_all_user();
//
//    ESP_LOGI(TAG, "Initialising LittlevGL");
//    lv_init();
//
//    ESP_LOGI(TAG, "Initialising ST7789");
//    lvgl_st7789_init();
//
//    ESP_LOGI(TAG, "Initialising LVGL driver");
//    lv_disp_drv_t disp;
//    lv_disp_drv_init(&disp);
//    disp.flush_cb = lvgl_st7789_flush;
//
//    ESP_LOGI(TAG, "Registering LVGL driver");
//    lv_disp_drv_register(&disp);
//
//
//    ESP_LOGI(TAG, "Registering LVGL tick callback");
//    esp_register_freertos_tick_hook(lv_tick_cb);
//
//    ESP_LOGI(TAG, "Starting LVGL main task");
//    xTaskCreate(lvgl_main_task, "lv_task", 8192, nullptr, 5, nullptr);
//
//    fp_add_controller add_controller;
//    add_controller.preform_enroll();
//    vTaskDelay(pdMS_TO_TICKS(5000));
//    add_controller.~fp_add_controller();
//
//    fp_verify_controller verify_controller;
//    verify_controller.verify_fp();
//    vTaskDelay(portMAX_DELAY);

}

