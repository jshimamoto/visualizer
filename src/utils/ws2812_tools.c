#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "utils/ws2812_tools.h"
#include "utils/ws2812_config.h"

void pio_set_sm_and_init_ws2812_program(PIO *pio, uint *sm, uint *offset, uint8_t gpio_pin) {
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, pio, sm, offset, gpio_pin, 1, true);
    hard_assert(success);
    ws2812_program_init(*pio, *sm, *offset, gpio_pin, 800000, IS_RGBW);
}

void fade_from_to(uint32_t from_color, uint32_t to_color, PIO pio, uint *sm_array) {
    uint8_t r_from = (from_color >> 8) & 0xFF;
    uint8_t g_from = (from_color >> 16) & 0xFF;
    uint8_t b_from = from_color & 0xFF;

    uint8_t r_to = (to_color >> 8) & 0xFF;
    uint8_t g_to = (to_color >> 16) & 0xFF;
    uint8_t b_to = to_color & 0xFF;

    for (int i = 0; i < FADE_STEPS; i++) {
        uint8_t r = r_from + ((r_to - r_from) * i / FADE_STEPS);
        uint8_t g = g_from + ((g_to - g_from) * i / FADE_STEPS);
        uint8_t b = b_from + ((b_to - b_from) * i / FADE_STEPS);

        for (int s = 0; s < NUM_STRIPS; s++) {
            for (int j = 0; j < NUM_PIXELS; j++) {
                put_pixel(pio, sm_array[s], urgb_u32(r, g, b));
            }
        }

        sleep_ms(1);
    }
}

void draw_visualizer_frame(PIO pio, uint *sm_array, uint8_t *band_energy_frame) {
    for (int i = 0; i < NUM_STRIPS; i++) {
        int energy_level = band_energy_frame[i];

        for (int j = 0; j < NUM_PIXELS; j++) {
            uint32_t color;
            if (j < energy_level) {
                color = urgb_u32(0x00, 0x10, 0x04);
            } else {
                color = urgb_u32(0x00, 0x00, 0x00);
            }
            put_pixel(pio, sm_array[i], color);
        }
    }
}

void update_energy_heights(uint8_t *new_energy_heights, uint8_t *current_heights, uint8_t decay_rate) {
    for (int i = 0; i < NUM_STRIPS; i++) {
        uint8_t new_strip_height = new_energy_heights[i];
        
        if (new_strip_height > current_heights[i]) {
            current_heights[i] = new_strip_height;
        } else if (current_heights[i] > 0) {
            if (current_heights[i] > decay_rate) {
                current_heights[i] -= decay_rate;
            } else {
                current_heights[i] = 0;
            }
        }
    }
}
