add_library(utils
    print_amplitude.c 
    pwm_led.c
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(utils
    pico_stdlib
    hardware_adc
    hardware_pwm
)
