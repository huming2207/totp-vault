file(GLOB_RECURSE LVGL_HEADERS "lib/lvgl/*.h"  "lib/*.h" "lib/hal/*.h")
file(GLOB_RECURSE LVGL_INCLUDES LIST_DIRECTORIES true "lib/lvgl" "lib/hal")
file(GLOB_RECURSE LVGL_SOURCES "lib/lvgl/*.c" "lib/hal/*.c" "lib/*.c")

file(GLOB_RECURSE GUI_INCLUDES LIST_DIRECTORIES true "view/include" "model/include" "controller/include")
file(GLOB_RECURSE GUI_SOURCES "view/*.c" "model/*.c" "controller/*.c")
file(GLOB_RECURSE GUI_HEADERS "view/*.h" "model/*.h" "controller/*.h")

set(COMPONENT_SRCS "esp_lvgl_main.c" ${LVGL_HEADERS} ${LVGL_SOURCES} ${GUI_SOURCES} ${GUI_HEADERS})
set(COMPONENT_ADD_INCLUDEDIRS "" ${LVGL_INCLUDES} ${GUI_INCLUDES})

register_component()