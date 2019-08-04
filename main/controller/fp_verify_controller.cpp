#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <fp1020a.h>

#include "fp_verify_controller.hpp"

fp_verify_controller::fp_verify_controller() : view()
{

}

void fp_verify_controller::verify_fp()
{
    auto ret = fp1020a_auth_user();
    if(ret == nullptr) {
        view.set_title_text("Timeout\nTry again!");
        view.set_icon(LV_SYMBOL_BELL);
        verify_fp();
    } else if(ret->level == FP1020A_USER_LEVEL_FAIL) {
        view.set_title_text("You're dodgy\nTry again!");
        view.set_icon(LV_SYMBOL_WARNING);
        verify_fp();
    } else {
        view.set_title_text("Success!");
        view.set_icon(LV_SYMBOL_OK);
        vTaskDelay(portMAX_DELAY);
    }
}


