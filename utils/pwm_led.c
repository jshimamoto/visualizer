#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#define ADC_INPUT 0
#define ADC_PIN 26
#define LED_GPIO 25

#define BASELINE_SAMPLES 5000
#define AMBIENT_THRESHOLD 400
#define AUDIO_BASELINE 600

uint16_t read_mic() {
    return adc_read();
}

void pwm_led_loop() {
    // Initialize all
    stdio_init_all();

    // ADC init
    adc_init();
    adc_gpio_init(ADC_PIN);       // Enable ADC on GP26 (ADC0)
    adc_select_input(ADC_INPUT);     // Select ADC0 (connected to GP26)

    // LED init
    gpio_init(LED_GPIO);
    gpio_set_dir(LED_GPIO, GPIO_OUT);

    // Measure baseline noise and calculate average
    printf("Calculating baseline");
    uint32_t baseline_sum = 0;
    for (int i = 0; i < BASELINE_SAMPLES; ++i) {
        baseline_sum += read_mic();
        sleep_ms(1);
    }
    uint16_t baseline_avg = baseline_sum / BASELINE_SAMPLES;
    printf("Baseline: %d\n", baseline_avg);

    // Main loop
    while (true) {
        uint16_t sample = read_mic();
        int16_t diff = sample - baseline_avg;
        
        if (diff < 0) diff = -diff;
        if (diff < AMBIENT_THRESHOLD) diff = 0;
        // uint8_t brightness = diff > AUDIO_BASELINE ? 255 : diff;

        if (diff > AUDIO_BASELINE) printf("Registered above baseline\n");
        gpio_put(LED_GPIO, diff > AUDIO_BASELINE ? 1 : 0);

        sleep_ms(10);
    }
}
