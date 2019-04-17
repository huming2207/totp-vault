file(GLOB_RECURSE LVGL_HEADERS "lib/lvgl/*.h"  "lib/*.h" "lib/hal/*.h")
file(GLOB_RECURSE LVGL_INCLUDES LIST_DIRECTORIES true "lib/lvgl" "lib/hal")
file(GLOB_RECURSE LVGL_SOURCES "lib/lvgl/*.c" "lib/hal/*.c")

set(COMPONENT_SRCS "esp_lvgl_main.c" ${LVGL_HEADERS} ${LVGL_SOURCES})
set(COMPONENT_ADD_INCLUDEDIRS "" ${LVGL_INCLUDES})

register_component()