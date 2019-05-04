file(GLOB_RECURSE LVGL_HEADERS "lib/lvgl/*.h"  "lib/*.h" "lib/hal/*.h" "lib/*.hpp")
file(GLOB_RECURSE LVGL_INCLUDES LIST_DIRECTORIES true "lib/lvgl" "lib/hal" "lib")
file(GLOB_RECURSE LVGL_SOURCES "lib/lvgl/*.c" "lib/hal/*.c" "lib/*.c" "lib/*.cpp")

file(GLOB_RECURSE GUI_INCLUDES LIST_DIRECTORIES true "view/include" "model/include" "controller/include")
file(GLOB_RECURSE GUI_SOURCES "view/*.cpp" "model/*.cpp" "controller/*.cpp")
file(GLOB_RECURSE GUI_HEADERS "view/*.hpp" "model/*.hpp" "controller/*.hpp")

set(COMPONENT_SRCS "lvgl_main.cpp" ${LVGL_HEADERS} ${LVGL_SOURCES} ${GUI_SOURCES} ${GUI_HEADERS})
set(COMPONENT_ADD_INCLUDEDIRS "" ${LVGL_INCLUDES} ${GUI_INCLUDES})

register_component()

# Disable narrowing conversion error, otherwise LittlevGL won't be compiled
component_compile_options("-Wno-narrowing")
