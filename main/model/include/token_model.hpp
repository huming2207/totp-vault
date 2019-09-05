#pragma once

#include <nvs_flash.h>

class token_model
{
    public:
        token_model();
        size_t count();
    private:
        nvs_handle_t handle = 0;
};

