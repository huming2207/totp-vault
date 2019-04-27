#pragma once

#include <memory>

#include <esp_err.h>
#include "fp_add_view.hpp"

class fp_add_controller
{
    public:
        fp_add_controller();
        esp_err_t preform_enroll();

    private:
        fp_add_view view;
};

