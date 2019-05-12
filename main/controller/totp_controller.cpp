#include <string>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <otp.h>

#include "totp_controller.hpp"
#include "totp_model.hpp"

totp_controller::totp_controller() : view()
{

}

void totp_controller::run_totp()
{
    totp_model model;
    auto model_list = model.get_totp_list();
    if(model_list.empty()) return;

    for(uint8_t idx = 0; idx < model_list.size(); idx++) {
        auto totp_tuple = model_list[idx];
        char code_str[6] = { '\0' };
        itoa(generate_token(totp_tuple.token), code_str, 10); // Convert to C string and write to
        view.set_label(idx, totp_tuple.title.c_str(), code_str);
    }
}

uint32_t totp_controller::generate_token(const std::vector<uint8_t> &key)
{
    return totp_generate((uint8_t *)key.data(), key.size());
}


