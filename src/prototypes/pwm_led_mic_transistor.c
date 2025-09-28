#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "prototypes/pwm_led_mic_transistor.h"

// GPIO
#define TRANS_BASE_PIN 20
#define ONBOARD_LED 25

// ADC
#define ADC_INPUT 0
#define ADC_PIN 26

// Sample settings
#define BASELINE_SAMPLES 5000
#define AMBIENT_THRESHOLD 1000
#define MAX_DIFF 2000

uint16_t read_mic() {
    return adc_read();
}

void setup_pwm(uint gpio_pin) {
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio_pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1.0f);
    pwm_init(slice, &config, true);
    pwm_set_gpio_level(gpio_pin, 0);
}

void main() {
    // Set onboard LED
    gpio_init(ONBOARD_LED);
    gpio_set_dir(ONBOARD_LED, GPIO_OUT);
    gpio_put(ONBOARD_LED, 1);
    
    // Initialize and config transistor GPIO
    gpio_init(TRANS_BASE_PIN);
    setup_pwm(TRANS_BASE_PIN);

    // ADC init
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_INPUT);

    // Set baseline audio input
    uint32_t baseline_total = 0;
    for (int i = 0; i < BASELINE_SAMPLES; i++) {
        baseline_total += read_mic();
        sleep_us(100);
    }
    uint16_t baseline = baseline_total / BASELINE_SAMPLES;

    uint16_t smoothed = 0;

    while (true) {
        // Filter out ambient noise
        uint16_t mic_input = read_mic();
        int16_t diff = mic_input - baseline;
        uint16_t mic_filtered = diff > AMBIENT_THRESHOLD ? diff : 0;
        if (mic_filtered > MAX_DIFF) mic_filtered = MAX_DIFF;

        float scaled = logf((float)mic_filtered + 1) / logf((float)MAX_DIFF + 1);
        uint16_t brightness = (uint16_t)(scaled * UINT16_MAX);

        smoothed = (smoothed * 7 + brightness) / 8;

        pwm_set_gpio_level(TRANS_BASE_PIN, smoothed);
        printf("Baseline: %d, Mic: %d, Diff: %d, Brightness: %d\n", baseline, mic_input, diff, brightness);
        sleep_ms(10);
    }
}
