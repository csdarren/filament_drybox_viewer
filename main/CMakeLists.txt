idf_component_register(
    SRCS "drybox_temp.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver esp_common cxx
)

target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)
