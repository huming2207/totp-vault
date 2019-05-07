#pragma once

#include "view.hpp"

typedef std::map<std::string, lvgl_obj_ptr> lvgl_obj_map;

class mapped_view : public view
{
    public:
        mapped_view() = default;
        lv_obj_t *get_obj_by_name(const std::string& name)
        {
            return obj_map[name].get();
        }

    protected:
        lvgl_obj_map obj_map{};

};