set(PROTOTYPE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

function(prepare_prototype name)
    add_executable(${name} ${name}.c)

    target_include_directories(${name} PRIVATE ${PROTOTYPE_INCLUDE_DIR})

    target_link_libraries(${name}
        pico_stdlib
        pico_time
        hardware_adc
        hardware_pwm
        hardware_pio
        kissfft
        utils
    )

    pico_enable_stdio_usb(${name} 1)
    pico_enable_stdio_uart(${name} 0)
    pico_add_extra_outputs(${name})
endfunction()

set(PROTOTYPE_LIST
    print_amplitude
    pwm_led_mic
    blink_led_transistor
    pwm_led_transistor
    pwm_led_mic_transistor
    fft_5_band
    ws2812_test
    ws2812_multi
    visualizer_4_strip
    visualizer_8_strip
)

foreach(proto ${PROTOTYPE_LIST})
    prepare_prototype(${proto})
endforeach()
