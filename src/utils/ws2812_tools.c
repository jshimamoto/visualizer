#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "utils/fft_tools.h"
#include "utils/ws2812_tools.h"
#include "utils/ws2812_config.h"

void pio_set_sm_and_init_ws2812_program(PIO *pio, uint *sm, uint *offset, uint8_t gpio_pin) {
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, pio, sm, offset, gpio_pin, 1, true);
    hard_assert(success);
    ws2812_program_init(*pio, *sm, *offset, gpio_pin, 800000, IS_RGBW);
}

// Fade color using a reference to a global color
void fade_from_to_global_color(volatile uint32_t *global_color, uint32_t from_color, uint32_t to_color) {
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

        *global_color = urgb_u32(r, g, b);

        sleep_ms(5);
    }
}

// Fade color directly on strip
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

        sleep_ms(2);
    }
}

// Draws the visualizer board based off of input strip height (8x35 matrix)
void draw_visualizer_frame(PIO *pio_array, uint *sm_array, uint8_t *height_frame, uint32_t color) {
    for (int i = 0; i < NUM_STRIPS; i++) {
        int energy_level = height_frame[i];
        PIO pio = pio_array[i];

        for (int j = 0; j < NUM_PIXELS; j++) {
            if (j < energy_level) {
                put_pixel(pio, sm_array[i], color);
            } else {
                put_pixel(pio, sm_array[i], urgb_u32(0x00, 0x00, 0x01));
            }
        }
    }
}

// Draws the visualizer board based off of input strip height (35x8 matrix)
void draw_visualizer_frame_landscape(PIO *pio_array, uint *sm_array, uint32_t frame[NUM_PIXELS][NUM_STRIPS], uint32_t color) {
    for (int strip = 0; strip < NUM_STRIPS; strip++) {
        PIO pio = pio_array[strip];
        uint sm = sm_array[strip];

        for (int px = 0; px < NUM_PIXELS; px++) {
            put_pixel(pio, sm_array[strip], frame[strip][px]);
        }
    }
}

// Deprecated
void update_energy_heights(uint8_t *new_band_energies, uint8_t *current_heights, uint8_t decay_rate) {
    for (int i = 0; i < NUM_STRIPS; i++) {
        uint8_t new_energy_height = new_band_energies[i];
        
        if (new_energy_height > current_heights[i]) {
            current_heights[i] = new_energy_height;
        } else if (current_heights[i] > 0) {
            if (current_heights[i] > decay_rate) {
                current_heights[i] -= decay_rate;
            } else {
                current_heights[i] = 0;
            }
        }
    }
}

// Converts the FFT band energies to LED pixel counts based off of strip length
// Increases height if greater than previous, otherwise starts the decay 
void update_energy_heights_fft(uint16_t *new_band_energies, uint8_t *current_heights, uint8_t decay_rate) {
    for (int i = 0; i < NUM_STRIPS; i++) {
        if (new_band_energies[i] > MAX_BAND_ENERGY) new_band_energies[i] = MAX_BAND_ENERGY;
        uint8_t normalized_height = normalize_energy_to_pixel_count(new_band_energies[i], MAX_BAND_ENERGY);
        
        if (normalized_height > current_heights[i]) {
            current_heights[i] = normalized_height;
        } else if (current_heights[i] > 0) {
            if (current_heights[i] > decay_rate) {
                current_heights[i] -= decay_rate;
            } else {
                current_heights[i] = 0;
            }
        }
    }
}

// Transposes a 8x35 matrix to a 35x8 for easy feeding into the animation function
void rotate_landscape_to_portrait(uint32_t raw_rows[NUM_PIXELS][NUM_STRIPS], uint32_t rotated[NUM_STRIPS][NUM_PIXELS]) {
    for (int row = 0; row < NUM_STRIPS; row++) {
        for (int col = 0; col < NUM_PIXELS; col++) {
            rotated[row][col] = raw_rows[NUM_PIXELS - 1 - col][row];
        }
    }
}

void build_animation_frame(uint16_t *new_band_energies, uint8_t *current_heights, uint8_t decay_rate) {
    if (FRAME_ORIENTATION == 0) {

    } else if (FRAME_ORIENTATION == 1) {
        for (int i = 0; i < NUM_STRIPS; i++) {
            if (new_band_energies[i] > MAX_BAND_ENERGY) new_band_energies[i] = MAX_BAND_ENERGY;
            uint8_t normalized_height = normalize_energy_to_pixel_count(new_band_energies[i], MAX_BAND_ENERGY);
            
            if (normalized_height > current_heights[i]) {
                current_heights[i] = normalized_height;
            } else if (current_heights[i] > 0) {
                if (current_heights[i] > decay_rate) {
                    current_heights[i] -= decay_rate;
                } else {
                    current_heights[i] = 0;
                }
            }
        }
    }
}

// Converts the FFT band energies to LED pixel counts based off of column height for 2D matrix
// Increases height if greater than previous, otherwise starts the decay 
void update_energy_heights_landscape(uint16_t *new_band_energies, uint8_t *current_heights, uint8_t decay_rate) {
    for (int i = 0; i < NUM_STRIPS; i++) {
        if (new_band_energies[i] > MAX_BAND_ENERGY) new_band_energies[i] = MAX_BAND_ENERGY;
        uint8_t normalized_height = normalize_energy_to_pixel_count(new_band_energies[i], MAX_BAND_ENERGY);
        
        if (normalized_height > current_heights[i]) {
            current_heights[i] = normalized_height;
        } else if (current_heights[i] > 0) {
            if (current_heights[i] > decay_rate) {
                current_heights[i] -= decay_rate;
            } else {
                current_heights[i] = 0;
            }
        }
    }
}
