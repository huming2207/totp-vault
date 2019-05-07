#pragma once

#include <list>
#include <vector>
#include "view.hpp"

typedef std::vector<lvgl_obj_ptr> lvgl_obj_tuple;
typedef std::vector<lvgl_obj_tuple> lvgl_obj_list;


class listed_view : public view
{
    public:
        listed_view() = default;

    protected:
        lvgl_obj_list obj_list{};

        // From: https://tristanbrindle.com/posts/beware-copies-initializer-list
        template <typename... Args>
        lvgl_obj_tuple make_obj_tuple(Args&&... args)
        {
            lvgl_obj_tuple tuple;
            tuple.reserve(sizeof...(Args));
            using arr_t = int[];
            (void) arr_t{0, (tuple.emplace_back(std::forward<Args>(args)), 0)...};
            return tuple;
        };
};
