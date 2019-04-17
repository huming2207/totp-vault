file(GLOB_RECURSE LVGL_HEADERS "lib/lvgl/*.h"  "lib/*.h" )
file(GLOB_RECURSE LVGL_INCLUDES LIST_DIRECTORIES true "lib/lvgl")
file(GLOB_RECURSE LVGL_SOURCES "lib/lvgl/*.c" )

set(COMPONENT_SRCS "esp_lvgl_main.c" ${LVGL_HEADERS} ${LVGL_SOURCES})
set(COMPONENT_ADD_INCLUDEDIRS "" ${LVGL_INCLUDES})

register_component()