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

// Simulated FFT energy data for 20 frames, two strips, 0–35 range
// Test data: 50 frames, 2 strips
// Values represent number of LEDs lit per strip (0–35)
uint8_t band_energies[50][2] = {
    {0, 0}, {5, 0}, {10, 3}, {15, 7}, {20, 10}, {25, 15}, {30, 18}, {35, 20},
    {30, 18}, {25, 15}, {20, 10}, {15, 7}, {10, 3}, {5, 0}, {0, 0}, {3, 5},
    {6, 10}, {12, 15}, {18, 20}, {24, 25}, {30, 28}, {35, 30}, {30, 28}, {25, 25},
    {20, 20}, {15, 15}, {10, 10}, {5, 5}, {0, 0}, {0, 3}, {0, 6}, {0, 12},
    {0, 18}, {0, 24}, {0, 30}, {0, 35}, {0, 30}, {0, 25}, {0, 20}, {0, 15},
    {0, 10}, {0, 5}, {0, 0}, {5, 0}, {10, 3}, {15, 7}, {20, 10}, {25, 15}, {30, 20}, {35, 25}
};


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

    uint8_t current_heights[NUM_STRIPS] = {0};

    while (true) {
        for (int frame = 0; frame < 50; frame++) {
            update_energy_heights(band_energies[frame], current_heights, 1);
            draw_visualizer_frame(pio, sm_array, current_heights);
            sleep_ms(25);
        }
    }
}
