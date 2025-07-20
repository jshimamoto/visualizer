#include "pico/stdlib.h"
#include "utils/led_tools.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

void light_onboard_led(uint8_t GPIO_PIN_NUMBER) {
    gpio_init(GPIO_PIN_NUMBER);
    gpio_set_dir(GPIO_PIN_NUMBER, GPIO_OUT);
    gpio_put(GPIO_PIN_NUMBER, 1);
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
