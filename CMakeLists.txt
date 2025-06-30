cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(visualize C CXX ASM)

pico_sdk_init()

add_subdirectory(src/utils)

add_executable(visualize
  src/main.c
)

target_link_libraries(visualize 
  pico_stdlib 
  hardware_adc 
  hardware_pwm
  utils
)

pico_enable_stdio_usb(visualize 1)
pico_enable_stdio_uart(visualize 0)

pico_add_extra_outputs(visualize)
