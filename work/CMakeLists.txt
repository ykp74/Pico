cmake_minimum_required(VERSION 3.12)
# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

set(projname "ykp74")

project(${projname} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_FREERTOS_SOURCE freertos/FreeRTOS-Kernel )

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

add_subdirectory(freertos)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(${projname}
        main.cpp
        ssd1306.cpp
        ad9833.cpp
        )

# pull in common dependencies
target_link_libraries(${projname} pico_stdlib hardware_i2c hardware_spi freertos)

# enable usb output, disable uart output
pico_enable_stdio_usb(${projname} 1)
pico_enable_stdio_uart(${projname} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${projname})