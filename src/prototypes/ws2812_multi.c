/**
 * LEDs per strip: 35
 * Max hex sum per LED: 21 (21.25) => round to 20
 * Total current per LED: 5mA
 * Total current per strip max: 175mA
 * Total current among all (8) strips: 1.4A
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "prototypes/ws2812_multi.h"
#include "utils/led_tools.h"
#include "utils/ws2812_tools.h"

#define NUM_PIXELS 35
#define NUM_STRIPS 2
#define MAX_HEX 0x14

#define WS2812_PINS {10, 11}

#define FADE_STEPS 500

void ws2812_multi() {
    stdio_init_all();
    light_onboard_led();

    PIO pio;
    uint gpio_pin_array[NUM_STRIPS] = WS2812_PINS; 
    uint sm_array[NUM_STRIPS];
    uint offset_array[NUM_STRIPS];

    for (int i = 0; i < NUM_STRIPS; i++) {
        pio_set_sm_and_init_ws2812_program(&pio, &sm_array[i], &offset_array[i], gpio_pin_array[i]);
    }

    while (true) {
        fade_from_to(red, green, pio, sm_array);
        fade_from_to(green, blue, pio, sm_array);
        fade_from_to(blue, red, pio, sm_array);
    }
}
