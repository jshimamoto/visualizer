#include <stdio.h>
#include "pico/stdlib.h"
#include "utils/pwm_led.h"

#define ADC_INPUT 0
#define ADC_PIN 26
#define LED_GPIO 25

int main() {
    stdio_init_all();

    pwm_led_loop();

    return 0;
}
