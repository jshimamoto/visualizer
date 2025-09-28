cmake_minimum_required(VERSION 3.13)

# Initialize project and pico-sdk
include(pico_sdk_import.cmake)
project(visualize C CXX ASM)
pico_sdk_init()

# Build mode: main | prototypes | all
set(BUILD_MODE "main" CACHE STRING "What to build: main | proto | all")
set_property(CACHE BUILD_MODE PROPERTY STRINGS main prototypes all)

# Add dependencies
add_subdirectory(src/utils)
add_subdirectory(lib/kissfft)

if(BUILD_MODE STREQUAL "main" OR BUILD_MODE STREQUAL "all")
  add_executable(visualize src/main.c)

  target_include_directories(visualize PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

  target_link_libraries(visualize 
    pico_stdlib
    pico_time
    hardware_adc
    hardware_pwm
    hardware_pio
    kissfft
    utils
  )

  pico_enable_stdio_usb(visualize 1)
  pico_enable_stdio_uart(visualize 0)
  pico_add_extra_outputs(visualize)
endif()

if(BUILD_MODE STREQUAL "proto" OR BUILD_MODE STREQUAL "all")
  add_subdirectory(src/prototypes)
endif()
