#pragma once

#include <functional>
#include <memory>
#include <map>

#include <lvgl.h>

typedef std::unique_ptr<lv_obj_t, std::function<void(lv_obj_t*)>> lvgl_obj_ptr;

class view
{
    public:
        static lvgl_obj_ptr make_object(lv_obj_t *lv_ptr)
        {
            return std::unique_ptr<lv_obj_t, void(*)(lv_obj_t*)>(lv_ptr, [](lv_obj_t * ptr){ lv_obj_del(ptr); });
        };

};