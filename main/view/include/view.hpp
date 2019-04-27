#pragma once

#include <memory>
#include <map>

#include <lvgl.h>

typedef std::map<std::string, std::unique_ptr<lv_obj_t>> lvgl_obj_map;

class view
{
    public:
        virtual lv_obj_t *get_object(const std::string& name) = 0;

};