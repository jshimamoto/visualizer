#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#define ADC_INPUT 0
#define ADC_PIN 26
#define LED_GPIO 25

int main() {
    // Initialize inputs and outputs
    stdio_init_all();
    adc_init();
    adc_gpio_init(ADC_PIN);       // Enable ADC on GP26 (ADC0)

    adc_select_input(ADC_INPUT);     // Select ADC0 (connected to GP26)
    gpio_set_function(LED_GPIO, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(LED_GPIO);
    pwm_set_wrap(slice_num, 255);
    pwm_set_enabled(slice_num, true);

    while (true) {
        uint16_t mic_val = adc_read();
        printf("Mic: %d\n", mic_val);

        uint8_t brightness = mic_val >> 4;


        pwm_set_chan_level(slice_num, pwm_gpio_to_channel(LED_GPIO), brightness);

        sleep_ms(10);
        // uint16_t min = 4095;
        // uint16_t max = 0;

        // for (int i = 0; i < 100; i++) {
        //     uint16_t val = adc_read();
        //     if (val < min) min = val;
        //     if (val > max) max = val;
        //     sleep_us(100);
        // }

        // uint16_t amplitude = max - min;  // 12-bit value (0–4095)
        // printf("Peak-to-peak amplitude: %d\n", amplitude);
        // sleep_ms(50);
    }
}
