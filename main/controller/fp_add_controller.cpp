#include <sstream>

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
    fp1020a_init();
}


esp_err_t fp_add_controller::preform_enroll()
{
    auto add_progress = view.get_object("add_progress");
    fp_ack_t ret = FP1020A_ACK_SUCCESS;

    // First round
    do {
        view.set_title_label_text("Recording 1/6");
        ret = fp1020a_add_fp_start(1, FP1020A_USER_LEVEL_1);
        if(ret == FP1020A_ACK_TRY_AGAIN) view.set_title_label_text("Please try again");
        else if(ret == FP1020A_ACK_SUCCESS) break;
        else {
            view.set_title_label_text("Unexpected error");
            fp1020a_remove_all_user(); // TODO: remove this later on, maybe
            vTaskDelay(portMAX_DELAY);
        }
    } while(ret == FP1020A_ACK_TRY_AGAIN);
    lv_bar_set_value_anim(add_progress, (int16_t)(100.0 / 6.0), 200);


    // Intermediate rounds
    for(uint8_t count = 2; count <= 5; count++) {
        do {
            std::stringstream buf;
            buf << "Recording " << (int)count << "/6";
            view.set_title_label_text(buf.str());


            ret = fp1020a_add_fp_intermediate(1, FP1020A_USER_LEVEL_1);
            if(ret == FP1020A_ACK_TRY_AGAIN) view.set_title_label_text("Please try again");
            else if(ret == FP1020A_ACK_SUCCESS) break;
            else {
                view.set_title_label_text("Unexpected error");
                fp1020a_remove_all_user(); // TODO: remove this later on, maybe
                vTaskDelay(portMAX_DELAY);
            }
        } while(ret == FP1020A_ACK_TRY_AGAIN);
        lv_bar_set_value_anim(add_progress, (int16_t)(100.0 / 6.0 * (double)count), 200);
    }

    // Last round
    do {
        view.set_title_label_text("Recording 6/6");

        ret = fp1020a_add_fp_end(1, FP1020A_USER_LEVEL_1);
        if(ret == FP1020A_ACK_TRY_AGAIN) view.set_title_label_text("Please try again");
        else if(ret == FP1020A_ACK_SUCCESS) break;
        else {
            view.set_title_label_text("Unexpected error");
            fp1020a_remove_all_user(); // TODO: remove this later on, maybe
            vTaskDelay(portMAX_DELAY);
        }
    } while(ret == FP1020A_ACK_TRY_AGAIN);
    lv_bar_set_value_anim(add_progress, 100, 200);

    view.set_title_label_text("Done!\n\n\n\n\n\nRecording deleted.\nTrust me, it's a demo");


    return ESP_OK;
}


fp_add_controller::~fp_add_controller()
{
    fp1020a_remove_all_user();
    fp1020a_deinit();
    vTaskDelay(portMAX_DELAY);
}


