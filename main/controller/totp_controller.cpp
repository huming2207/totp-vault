#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "totp_controller.hpp"

totp_controller::totp_controller() : view()
{

}

void totp_controller::run_totp()
{
    vTaskDelay(pdMS_TO_TICKS(30000));
}


