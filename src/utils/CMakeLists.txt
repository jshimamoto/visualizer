add_library(utils
    led_tools.c
    fft_tools.c
    mic_tools.c
    ws2812_tools.c
    aux_tools.c
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(utils
    pico_stdlib
    pico_time
    hardware_adc
    hardware_pwm
    hardware_pio
    kissfft
)

pico_generate_pio_header(utils ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)
