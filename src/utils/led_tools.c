#include "pico/stdlib.h"
#include "utils/led_tools.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

void light_onboard_led(uint8_t GPIO_PIN_NUMBER) {
    gpio_init(GPIO_PIN_NUMBER);
    gpio_set_dir(GPIO_PIN_NUMBER, GPIO_OUT);
    gpio_put(GPIO_PIN_NUMBER, 1);
}

void init_transistor_pin_out(uint GPIO_PIN_NUMBER) {
    gpio_init(GPIO_PIN_NUMBER);
    gpio_set_dir(GPIO_PIN_NUMBER, GPIO_OUT);
}

void setup_pwm(uint GPIO_PIN) {
    gpio_set_function(GPIO_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(GPIO_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1.0f);
    pwm_init(slice, &config, true);
    pwm_set_gpio_level(GPIO_PIN, 0);
}

uint16_t read_mic() {
    return adc_read();
}

uint16_t get_baseline_mic_input(uint16_t NUM_SAMPLES) {
    uint32_t baseline_total = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        baseline_total += read_mic();
        sleep_us(100);
    }
    return (uint16_t)(baseline_total / NUM_SAMPLES);
}
