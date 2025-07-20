cmake_minimum_required(VERSION 3.13)

# Initialize project and pico-sdk
include(pico_sdk_import.cmake)
project(visualize C CXX ASM)
pico_sdk_init()

# Add dependencies
add_subdirectory(src/utils)
add_subdirectory(lib/kissfft)

# Build
add_executable(visualize
  src/main.c
)

target_link_libraries(visualize 
  pico_stdlib 
  hardware_adc 
  hardware_pwm
  utils
  kissfft
)

# Config
pico_enable_stdio_usb(visualize 1)
pico_enable_stdio_uart(visualize 0)

pico_add_extra_outputs(visualize)
