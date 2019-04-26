#include <esp_log.h>
#include <esp_err.h>

#include <fp1020a.h>

#include "fp_add_view.h"
#include "fp_add_controller.h"

#define TAG "fp_add_ctrl"

static lv_obj_t *add_progress = NULL;
static lv_obj_t *add_label = NULL;

esp_err_t fp_add_screen_init()
{
    if((add_progress = view_render_add_label()) == NULL) return ESP_FAIL;
    if((add_label = view_render_add_progress()) == NULL) return ESP_FAIL;

    return ESP_OK;
}

esp_err_t fp_add_preform_enrol()
{
    ESP_LOGI(TAG, "Start recording");
    fp_ack_t ret = fp1020a_add_fp_start(1, FP1020A_USER_LEVEL_1); // TODO: change the user ID after user model is done
    if(ret == FP1020A_ACK_TIMEOUT || ret == FP1020A_ACK_TRY_AGAIN) return fp_add_preform_enrol();
    if(ret != FP1020A_ACK_SUCCESS) return ESP_FAIL;
    ESP_LOGI(TAG, "Done step 1/6");

    lv_preload_set_arc_length(add_progress, 60); // 60 * 1 = 36, the first pass

    for(uint8_t count = 2; count < 6; count += 1) { // The 2nd to 5th passes
        ret = fp1020a_add_fp_intermediate(1, FP1020A_USER_LEVEL_1);
        if(ret == FP1020A_ACK_TIMEOUT || ret == FP1020A_ACK_TRY_AGAIN) return fp_add_preform_enrol();
        if(ret != FP1020A_ACK_SUCCESS) return ESP_FAIL;
        ESP_LOGI(TAG, "Done step %u/6", count);
        lv_preload_set_arc_length(add_progress, 60 * count);
    }

    ret = fp1020a_add_fp_end(1, FP1020A_USER_LEVEL_1);
    if(ret == FP1020A_ACK_TIMEOUT || ret == FP1020A_ACK_TRY_AGAIN) return fp_add_preform_enrol();
    if(ret != FP1020A_ACK_SUCCESS) return ESP_FAIL;
    ESP_LOGI(TAG, "Done step 6/6");
    lv_preload_set_arc_length(add_progress, 360);
    lv_label_set_text(add_label, "Add successful");

    return ESP_OK;
}

