idf_component_register(
    SRCS 
        "terrarium_monitor.c"
        "sensor_manager.c"
        "alarm_manager.c"
        "environmental_control.c"
    INCLUDE_DIRS 
        "include"
    REQUIRES 
        driver
        esp_timer
        nvs_flash
        json
        freertos
        main
)