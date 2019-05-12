#pragma once

#include "fp_verify_view.hpp"

class fp_verify_controller
{
public:
    fp_verify_controller();
    void verify_fp();

private:
    fp_verify_view view;
};