#include <esp_log.h>
#include <esp_err.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <fp1020a.h>


#include "fp_add_view.hpp"
#include "fp_add_controller.h"

#define TAG "fp_add_ctrl"

static lv_obj_t *add_progress = NULL;
static lv_obj_t *add_label = NULL;

esp_err_t fp_add_screen_init()
{
    if((add_progress = view_render_add_progress()) == NULL) return ESP_FAIL;
    if((add_label = view_render_add_label()) == NULL) return ESP_FAIL;

    return ESP_OK;
}

esp_err_t fp_add_preform_enrol()
{


    for(uint8_t count = 1; count <= 6; count++) {
        lv_bar_set_value_anim(add_progress, (int16_t)(100.0 / 6.0 * (double)count), 200);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    return ESP_OK;
}

