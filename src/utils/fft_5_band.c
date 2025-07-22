#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "utils/fft_5_band.h"
#include "utils/led_tools.h"
#include "kiss_fftr.h"

// GPIO
#define BAND_1_TRANS_PIN 2
#define BAND_2_TRANS_PIN 5
#define BAND_3_TRANS_PIN 6
#define BAND_4_TRANS_PIN 10
#define BAND_5_TRANS_PIN 13
#define ONBOARD_LED 25

// ADC
#define ADC_INPUT 0
#define ADC_PIN 26

// Sample and FFT settings
#define SAMPLE_RATE         10000
#define FFT_SIZE            128
#define NUM_BANDS           5
#define BASELINE_SAMPLES    5000
#define AMBIENT_THRESHOLD   1000
#define MAX_DIFF            2000

// kiss_fftr_cfg fft_cfg;
// int16_t adc_buffer[FFT_SIZE];
// kiss_fft_cpx fft_out[FFT_SIZE / 2 + 1];

void five_band_fft_loop() {
    light_onboard_led(ONBOARD_LED);
    
    // ADC init
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_INPUT);

    // Initialize and config transistor GPIO
    init_transistor_pin_out(BAND_1_TRANS_PIN);
    init_transistor_pin_out(BAND_2_TRANS_PIN);
    init_transistor_pin_out(BAND_3_TRANS_PIN);
    init_transistor_pin_out(BAND_4_TRANS_PIN);
    init_transistor_pin_out(BAND_5_TRANS_PIN);
    // setup_pwm(TRANS_BASE_PIN);

    // Set baseline audio input
    // uint16_t baseline = get_baseline_mic_input(BASELINE_SAMPLES);

    // uint16_t smoothed = 0;

    while (true) {
        // Filter out ambient noise
        // uint16_t mic_input = read_mic();
        // int16_t diff = mic_input - baseline;
        // uint16_t mic_filtered = diff > AMBIENT_THRESHOLD ? diff : 0;
        // if (mic_filtered > MAX_DIFF) mic_filtered = MAX_DIFF;

        // float scaled = logf((float)mic_filtered + 1) / logf((float)MAX_DIFF + 1);
        // uint16_t brightness = (uint16_t)(scaled * UINT16_MAX);

        // smoothed = (smoothed * 7 + brightness) / 8;

        // pwm_set_gpio_level(TRANS_BASE_PIN, smoothed);
        // printf("Baseline: %d, Mic: %d, Diff: %d, Brightness: %d\n", baseline, mic_input, diff, brightness);
        // sleep_ms(10);

        gpio_put(BAND_1_TRANS_PIN, 1);
        gpio_put(BAND_2_TRANS_PIN, 1);
        gpio_put(BAND_3_TRANS_PIN, 1);
        gpio_put(BAND_4_TRANS_PIN, 1);
        gpio_put(BAND_5_TRANS_PIN, 1);
        sleep_ms(1000);

        gpio_put(BAND_1_TRANS_PIN, 0);
        gpio_put(BAND_2_TRANS_PIN, 0);
        gpio_put(BAND_3_TRANS_PIN, 0);
        gpio_put(BAND_4_TRANS_PIN, 0);
        gpio_put(BAND_5_TRANS_PIN, 0);
        sleep_ms(1000);
    }
}
