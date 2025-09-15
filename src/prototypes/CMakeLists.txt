add_library(prototypes
    print_amplitude.c 
    pwm_led_mic.c
    blink_led_transistor.c
    pwm_led_transistor.c
    pwm_led_mic_transistor.c
    fft_5_band.c
    ws2812_test.c
    ws2812_multi.c
    visualizer_4_strip.c
)

target_include_directories(prototypes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(prototypes
    pico_stdlib
    pico_time
    hardware_adc
    hardware_pwm
    hardware_pio
    kissfft
    utils
)
