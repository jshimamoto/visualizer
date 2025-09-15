#include <stdio.h>
#include "pico/stdlib.h"
#include "prototypes/ws2812_multi.h"
#include "prototypes/print_amplitude.h"
#include "utils/led_tools.h"

int main() {
    stdio_init_all();
    light_onboard_led();
    sleep_ms(2000);

    print_amplitude_loop();

    return 0;
}
