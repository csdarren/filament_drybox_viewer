idf_component_register(
    SRCS "drybox_temp.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_common esp_lcd
)

# target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)
