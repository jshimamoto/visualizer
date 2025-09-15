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

#define WS2812_PINS {10, 11, 12, 13}

uint8_t band_energies[50][4] = {
    {0, 0, 0, 0}, {5, 0, 2, 1}, {10, 3, 5, 3}, {15, 7, 8, 6}, {20, 10, 12, 9}, {25, 15, 18, 14}, {30, 18, 22, 18}, {35, 20, 25, 22},
    {30, 18, 22, 18}, {25, 15, 18, 14}, {20, 10, 12, 9}, {15, 7, 8, 6}, {10, 3, 5, 3}, {5, 0, 2, 1}, {0, 0, 0, 0}, {3, 5, 0, 2},
    {6, 10, 3, 5}, {12, 15, 8, 10}, {18, 20, 12, 15}, {24, 25, 18, 20}, {30, 28, 22, 24}, {35, 30, 25, 28}, {30, 28, 22, 24}, {25, 25, 18, 20},
    {20, 20, 12, 15}, {15, 15, 8, 10}, {10, 10, 5, 6}, {5, 5, 2, 3}, {0, 0, 0, 0}, {0, 3, 0, 1}, {0, 6, 2, 3}, {0, 12, 5, 6},
    {0, 18, 8, 10}, {0, 24, 12, 15}, {0, 30, 18, 20}, {0, 35, 25, 28}, {0, 30, 22, 24}, {0, 25, 18, 20}, {0, 20, 12, 15}, {0, 15, 8, 10},
    {0, 10, 5, 6}, {0, 5, 2, 3}, {0, 0, 0, 0}, {5, 0, 3, 1}, {10, 3, 6, 3}, {15, 7, 12, 6}, {20, 10, 18, 10}, {25, 15, 22, 15}, {30, 20, 28, 20}, {35, 25, 30, 25}
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
    uint32_t colors[10] = {
        urgb_u32(0x14, 0x00, 0x00), // Pure red 20+0+0=20
        urgb_u32(0x00, 0x14, 0x00), // Pure green 0+20+0=20
        urgb_u32(0x00, 0x00, 0x14), // Pure blue 0+0+20=20
        urgb_u32(0x0A, 0x0A, 0x00), // Red+Green 10+10+0=20
        urgb_u32(0x0A, 0x00, 0x0A), // Red+Blue 10+0+10=20
        urgb_u32(0x00, 0x0A, 0x0A), // Green+Blue 0+10+10=20
        urgb_u32(0x08, 0x06, 0x06), // Mixed RGB 8+6+6=20
        urgb_u32(0x06, 0x08, 0x06), // Mixed RGB 6+8+6=20
        urgb_u32(0x06, 0x06, 0x08), // Mixed RGB 6+6+8=20
        urgb_u32(0x07, 0x07, 0x06)  // Mixed RGB 7+7+6=20
    };
    uint8_t color_index = 0;

    while (true) {
        for (int frame = 0; frame < 50; frame++) {
            update_energy_heights(band_energies[frame], current_heights, 1);
            draw_visualizer_frame(pio, sm_array, current_heights, colors[color_index]);
            sleep_ms(25);
        }
        color_index++;
        if (color_index > 9) {
            color_index = 0;
        }
    }
}
