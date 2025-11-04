# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/opt/esp-idf/components/bootloader/subproject"
  "/home/dhall/dev/drybox_temp/build/bootloader"
  "/home/dhall/dev/drybox_temp/build/bootloader-prefix"
  "/home/dhall/dev/drybox_temp/build/bootloader-prefix/tmp"
  "/home/dhall/dev/drybox_temp/build/bootloader-prefix/src/bootloader-stamp"
  "/home/dhall/dev/drybox_temp/build/bootloader-prefix/src"
  "/home/dhall/dev/drybox_temp/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/dhall/dev/drybox_temp/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/dhall/dev/drybox_temp/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
