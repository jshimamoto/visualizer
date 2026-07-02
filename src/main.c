/**
 * LEDs per strip: 35
 * Max hex sum per LED: 21.25 => round to 20
 * Total current per LED: 5mA
 * Total current per strip max: 175mA
 * Total current among all (8) strips: 1.4A
 */

// Standard C imports
#include <stdio.h>
#include <stdlib.h>

// Pico SDK
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"

// Utils
#include "utils/fft_tools.h"
#include "utils/mic_tools.h"
#include "utils/led_tools.h"
#include "utils/ws2812_tools.h"
#include "utils/aux_tools.h"
#include "utils/sampling_tools.h"
#include "utils/led_animations.h"

// File header
#include "main.h"

// Color
static uint32_t current_color;
static int color_index = 0;
static int fade_step = 0;

// Animation Logic
void visualizer_landscape() {
    stdio_init_all();

    // ADC init for input
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_INPUT);

    // PIO and GPIO set up
    PIO pio0_instance = pio0;
    uint gpio_pin_array[NUM_CHAINS] = WS2812_PINS_4; 
    uint sm_array[NUM_CHAINS];
    uint offset_array[NUM_CHAINS];
  
    for (int i = 0; i < NUM_CHAINS; i++) {
        pio_set_sm_and_init_ws2812_program(&pio0_instance, &sm_array[i], &offset_array[i], gpio_pin_array[i]);
    }

    // Animation set up
    uint16_t fft_band_energies[NUM_DISTINCT_BARS] = {0};
    uint8_t current_heights[NUM_DISTINCT_BARS] = {0};
    uint8_t display_heights [TOTAL_VIS_BARS] = {0};
    static uint32_t animation_frame[TOTAL_VIS_BARS][VIS_BAR_HEIGHT];
    static uint32_t a_frame_normalized[NUM_STRIPS][NUM_PIXELS];
    static uint32_t a_frame_snakified[NUM_CHAINS][NUM_PIXELS_IN_CHAIN];


    current_color = urgb_u32(color_cycle[0].r, color_cycle[0].g, color_cycle[0].b);

    while (true) {
        if (new_data_ready) {
            for (int i = 0; i < NUM_DISTINCT_BARS; i++) {
                fft_band_energies[i] = read_buffer[i];
            }
        }
        
        uint8_t new_heights[NUM_DISTINCT_BARS] = {0};
        normalize_band_energy_to_frame_height(fft_band_energies, new_heights, MAX_BAND_ENERGY);
        update_frame_heights(new_heights, current_heights, 1);

        // Right side (0–17)
        for (int i = 0; i < 18; i++) {
            display_heights[i] = new_heights[i];
        }

        // Right side (mirror: 18–34)
        for (int i = 0; i < 17; i++) {
            display_heights[18 + i] = new_heights[16 - i];
        }

        animate_bar_height_color(display_heights, animation_frame);
        // fade_color(&color_index, &fade_step, &current_color);
        // animate_single_color(display_heights, animation_frame, current_color);

        // Rendering
        rotate_landscape_to_portrait(animation_frame, a_frame_normalized);
        snakify_animation_frame(a_frame_normalized, a_frame_snakified);
        draw_visualizer_frame_matrix_snake(pio0_instance, sm_array, a_frame_snakified);
        
        sleep_ms(10);
    }
}

int main() {
    stdio_init_all();
    light_onboard_led();
    sleep_ms(2000);
    init_buffers();

    multicore_launch_core1(sampling_core);
    visualizer_landscape();

    return 0;
}
