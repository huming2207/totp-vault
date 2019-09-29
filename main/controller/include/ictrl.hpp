#pragma once

#include <m5stickc.hpp>

class ictrl
{
    public:
        ictrl() : bsp(m5stickc::get_bsp())
        {

        };

    protected:
        m5stickc& bsp;
};
