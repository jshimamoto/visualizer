#include <stdio.h>
#include "pico/stdlib.h"
#include "prototypes/pwm_led_mic_transistor.h"

int main() {
    stdio_init_all();
    sleep_ms(2000);

    pwm_led_mic_transistor();

    return 0;
}
