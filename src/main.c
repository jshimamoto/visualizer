#include <stdio.h>
#include "pico/stdlib.h"
#include "utils/fft_5_band.h"
#include "utils/pwm_led_mic_transistor.h"

int main() {
    stdio_init_all();
    
    pwm_led_mic_transistor();

    return 0;
}
