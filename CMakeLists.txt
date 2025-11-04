# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(drybox_temp)

### This went into main/ before, moved it here so I could have idf_component.yml in root instead of main/

# add_compile_options(-Wno-format)
