#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <esp_freertos_hooks.h>
#include <esp_log.h>

#include "lvgl.h"

#include "lvgl_hal_st7735r.h"

#include <m5stickc.hpp>
#include <otp_key.hpp>
#include <token_vector.hpp>

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

    auto& bsp = m5stickc::get_bsp();
    bsp.load_time();

    auto& token_vec = token_vector::instance();
    token_vec.emplace_back(otp_key("otpauth://totp/ACME%20Co:lol@example.com?"
                                   "secret=EPTS75A5IXCAU6LX33JM56PX2AZEHZ7V&"
                                   "issuer=ACME%20Co&algorithm=SHA1&digits=6&period=30"));

    token_vec.emplace_back(otp_key("otpauth://totp/lol:wat@sg.su?secret=H5ZUVEYU5NRVYOLYJZL553QJIEPWJJMM"
                                   "&issuer=SmartGuude&algorithm=SHA1&digits=6&period=30"));

    token_vec.emplace_back(otp_key("otpauth://totp/Oh:yes@example.com?secret=M6KPCLEQWANQDZGPSQB47BSDFZWSVLET"
                                   "&issuer=Oh&algorithm=SHA1&digits=6&period=30"));

    // NVS Write operation works if LVGL is not initialised
    ESP_ERROR_CHECK(token_vec.save());

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

    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 100, 50);                          /*Set its size*/

    lv_obj_t * label = lv_label_create(btn, NULL);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");


    // Uncomment here to crash
    // ESP_ERROR_CHECK(token_vec.save());


    ESP_LOGI(TAG, "Free heap finish parsing: %u, Max heap block: %u",
             heap_caps_get_free_size(MALLOC_CAP_8BIT), heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

    vTaskDelay(portMAX_DELAY);
}

