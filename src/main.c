#include <stdio.h>
#include "pico/stdlib.h"
#include "prototypes/visualizer_4_strip.h"
#include "prototypes/print_amplitude.h"
#include "utils/led_tools.h"

int main() {
    stdio_init_all();
    light_onboard_led();
    sleep_ms(2000);

    visualizer_4_strip();

    return 0;
}
