idf_component_register(
    SRCS 
        "lvgl_component.c"
        "lvgl_display.c"
        "lvgl_touch.c"
        "ui/dashboard.c"
        "ui/animals_ui.c"
        "ui/terrariums_ui.c"
        "ui/stocks_ui.c"
        "ui/settings_ui.c"
    INCLUDE_DIRS 
        "include"
        "ui"
        "."
    REQUIRES 
        driver
        esp_timer
        freertos
        lvgl
        main
)

# Configuration LVGL
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    LV_CONF_INCLUDE_SIMPLE=1
    LV_COLOR_DEPTH=16
    LV_COLOR_16_SWAP=1
    LV_MEM_CUSTOM=1
    LV_TICK_CUSTOM=1
)