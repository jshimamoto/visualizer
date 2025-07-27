add_library(utils
    print_amplitude.c 
    pwm_led_mic.c
    blink_led_transistor.c
    pwm_led_transistor.c
    pwm_led_mic_transistor.c
    fft_5_band.c
    led_tools.c
    fft_tools.c
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(utils
    pico_stdlib
    hardware_adc
    hardware_pwm
    kissfft
)
