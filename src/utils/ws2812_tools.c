#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "utils/fft_tools.h"
#include "utils/ws2812_tools.h"
#include "utils/ws2812_config.h"

// ============================================================================================================================================
// PIO ========================================================================================================================================
// ============================================================================================================================================
void pio_set_sm_and_init_ws2812_program(PIO *pio, uint *sm, uint *offset, uint8_t gpio_pin) {
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, pio, sm, offset, gpio_pin, 1, true);
    hard_assert(success);
    ws2812_program_init(*pio, *sm, *offset, gpio_pin, 800000, IS_RGBW);
}


// ============================================================================================================================================
// MATRIX =====================================================================================================================================
// ============================================================================================================================================
// Transposes a 8x35 matrix to a 35x8 for easy feeding into the animation function
void rotate_landscape_to_portrait(uint32_t raw_rows[NUM_PIXELS][NUM_STRIPS], uint32_t rotated[NUM_STRIPS][NUM_PIXELS]) {
    for (int row = 0; row < NUM_STRIPS; row++) {
        for (int col = 0; col < NUM_PIXELS; col++) {
            rotated[row][col] = raw_rows[NUM_PIXELS - 1 - col][row];
        }
    }
}

// ============================================================================================================================================
// ANIMATION ==================================================================================================================================
// ============================================================================================================================================

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

// Converts the energy strength array into visualizer vertical bar height
uint8_t normalize_band_energy_to_frame_height(uint32_t *energy_array, uint8_t *frame_heights, uint16_t max_energy) {
    for (int i = 0; i < VISUALIZER_COLS; i++) {
        frame_heights[i] = (uint8_t) (VISUALIZER_ROWS * energy_array[i] / max_energy);
    }
}

void update_frame_heights(uint16_t *new_frame_heights, uint8_t *current_frame_heights, uint8_t decay_rate) {
    for (int i = 0; i < VISUALIZER_COLS; i++) {
        if (new_frame_heights[i] >= current_frame_heights[i]) {
            current_frame_heights[i] = new_frame_heights[i];
        } else if (current_frame_heights[i] > 0) {
            if (current_frame_heights[i] > decay_rate) {
                current_frame_heights[i] -= decay_rate;
            } else {
                current_frame_heights[i] = 0;
            }
        }
    }
}

// Writes to the input matrix for the visualizer grid for each pixel
void build_animation_frame(uint8_t *current_frame_heights, uint32_t *animation_frame[VISUALIZER_COLS][VISUALIZER_ROWS], uint32_t color) {
    for (int col = 0; col < VISUALIZER_COLS; col++) {
        for (int bar_pixel = 0; bar_pixel < VISUALIZER_ROWS; bar_pixel++) {
            if (bar_pixel > current_frame_heights[col]) {
                animation_frame[col][bar_pixel] = urgb_u32(0x00, 0x00, 0x01);
            } else {
                animation_frame[col][bar_pixel] = color;
            }
        }
    }
}

// Draws the visualizer board from input animation frame matrix
void draw_visualizer_frame_new(PIO *pio_array, uint *sm_array, uint32_t *animation_frame[VISUALIZER_COLS][VISUALIZER_ROWS]) {
    for (int col = 0; col < VISUALIZER_COLS; col++) {
        PIO pio = pio_array[col];
        for (int bar_pixel = 0; bar_pixel < VISUALIZER_ROWS; bar_pixel++) {
            put_pixel(pio, sm_array[col], animation_frame[col][bar_pixel]);
        }
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

// ============================================================================================================================================
// COLOR CONTROL ==============================================================================================================================
// ============================================================================================================================================
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