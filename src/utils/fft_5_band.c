#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "utils/fft_5_band.h"
#include "utils/led_tools.h"
#include "kiss_fftr.h"
#include "fft_tools.h"
#include "mic_tools.h"

// GPIO
#define BAND_1_TRANS_PIN 2
#define BAND_2_TRANS_PIN 5
#define BAND_3_TRANS_PIN 6
#define BAND_4_TRANS_PIN 10
#define BAND_5_TRANS_PIN 13
#define ONBOARD_LED      25

// ADC
#define ADC_INPUT 0
#define ADC_PIN 26

// Mic Input
#define BASELINE_SAMPLES    5000

void five_band_fft_loop() {
    light_onboard_led(ONBOARD_LED);
    
    // ADC init
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_INPUT);

    // Initialize and config transistor GPIO
    // setup_pwm(TRANS_BASE_PIN);

    // Set baseline audio input
    uint16_t baseline = get_baseline_mic_input(BASELINE_SAMPLES);

    uint16_t smoothed = 0;

    while (true) {
        // Filter out ambient noise
        uint16_t mic_filtered = get_mic_output_filtered(baseline);
        
        // Set PWM brightness
        uint16_t brightness = get_pwm_brightness(mic_filtered);
        smoothed = (smoothed * 7 + brightness) / 8;

        // pwm_set_gpio_level(TRANS_BASE_PIN, smoothed);
        // printf("Baseline: %d, Mic: %d, Diff: %d, Brightness: %d\n", baseline, mic_input, diff, brightness);
        sleep_ms(10);
    }
}
