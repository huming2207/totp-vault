#include <esp_log.h>
#include <esp_err.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <fp1020a.h>


#include "fp_add_view.hpp"
#include "fp_add_controller.hpp"

#define TAG "fp_add_ctrl"

fp_add_controller::fp_add_controller() : view()
{

}


esp_err_t fp_add_controller::preform_enroll()
{
    auto add_progress = view.get_object("add_progress");

    for(uint8_t count = 1; count <= 6; count++) {
        lv_bar_set_value_anim(add_progress, (int16_t)(100.0 / 6.0 * (double)count), 200);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    return ESP_OK;
}


