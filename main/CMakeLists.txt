idf_component_register(SRCS "app.c"
                    INCLUDE_DIRS "."
                    REQUIRES led_strip driver esp_driver_i2s esp_driver_rmt)
