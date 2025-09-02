#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "prototypes/pwm_led.h"

#define ADC_INPUT 0
#define ADC_PIN 26
#define LED_GPIO 25

#define BASELINE_SAMPLES 5000
#define AMBIENT_THRESHOLD 400
#define AUDIO_BASELINE 600
#define MAX_DIFF 1000

uint16_t read_mic() {
    return adc_read();
}

void setup_pwm(uint gpio_pin) {
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio_pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f);
    pwm_init(slice, &config, true);
    pwm_set_gpio_level(gpio_pin, 0);
}

void pwm_led_mic_loop() {
    // Initialize all
    stdio_init_all();

    // ADC init
    adc_init();
    adc_gpio_init(ADC_PIN);       // Enable ADC on GP26 (ADC0)
    adc_select_input(ADC_INPUT);     // Select ADC0 (connected to GP26)

    // PWM LED init
    setup_pwm(LED_GPIO);

    // Measure baseline noise and calculate average
    uint32_t baseline_sum = 0;
    for (int i = 0; i < BASELINE_SAMPLES; ++i) {
        baseline_sum += read_mic();
        sleep_ms(1);
    }
    uint16_t baseline_avg = baseline_sum / BASELINE_SAMPLES;

    // Main loop
    while (true) {
        uint16_t sample = read_mic();
        printf("%d\n", sample);
        int16_t diff = sample - baseline_avg;
        
        if (diff < 0) diff = -diff;
        if (diff < AMBIENT_THRESHOLD) diff = 0;

        uint16_t brightness = (diff > MAX_DIFF) ? 65535 : (diff * 65535 / MAX_DIFF);

        pwm_set_gpio_level(LED_GPIO, brightness);

        sleep_us(125);
    }
}
