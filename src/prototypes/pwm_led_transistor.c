#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "prototypes/pwm_led_transistor.h"

#define TRANS_BASE_PIN 20
#define ONBOARD_LED 25

void setup_pwm(uint gpio_pin) {
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio_pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1.0f);
    pwm_init(slice, &config, true);
    pwm_set_gpio_level(gpio_pin, 0);
}

void pwm_led_transistor() {
    gpio_init(TRANS_BASE_PIN);

    gpio_init(ONBOARD_LED);
    gpio_set_dir(ONBOARD_LED, GPIO_OUT);

    gpio_put(ONBOARD_LED, 1);
    setup_pwm(TRANS_BASE_PIN);

    int32_t brightness = 0;
    int8_t direction = 1;

    while (true) {
        pwm_set_gpio_level(TRANS_BASE_PIN, brightness);
        brightness += (direction * UINT16_MAX) / 100;

        if (brightness >= UINT16_MAX) direction = -1;
        if (brightness <= 0) direction = 1;

        sleep_ms(5);
    }
}
