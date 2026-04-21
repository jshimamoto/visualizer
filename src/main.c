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

// File header
#include "main.h"

// Color changing thread
volatile uint32_t current_color;
void change_color_core() {
    const uint32_t color_cycle[] = {
        urgb_u32(0x14, 0x00, 0x00), // red
        urgb_u32(0x00, 0x14, 0x00), // green
        urgb_u32(0x00, 0x00, 0x14), // blue
        urgb_u32(0x0A, 0x0A, 0x00), // yellow
        urgb_u32(0x0A, 0x00, 0x0A), // magenta
        urgb_u32(0x00, 0x0A, 0x0A)  // cyan
    };
    const int num_colors = sizeof(color_cycle) / sizeof(color_cycle[0]);

    int i = 0;
    while (true) {
        int next = (i + 1) % num_colors;
        fade_from_to_global_color(&current_color, color_cycle[i], color_cycle[next]);
        i = (i + 1) % num_colors;
    }
}

// Animation Logic
void visualizer_landscape() {
    stdio_init_all();

    // ADC init for mic input
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_INPUT);

    // PIO and GPIO set up
    PIO pio0_instance = pio0;
    PIO pio1_instance = pio1;
    PIO pio_array[NUM_STRIPS] = {pio0, pio0, pio0, pio0, pio1, pio1, pio1, pio1};
    uint gpio_pin_array[NUM_STRIPS] = WS2812_PINS_8; 
    uint sm_array[NUM_STRIPS];
    uint offset_array[NUM_STRIPS];
  
    for (int i = 0; i < NUM_STRIPS; i++) {
        PIO selected_pio;
        if (i < 4) {
            selected_pio = pio0_instance;   // First 4 strips on PIO0
        } else {
            selected_pio = pio1_instance;   // Remaining strips on PIO1
        }

        pio_set_sm_and_init_ws2812_program(&selected_pio, &sm_array[i], &offset_array[i], gpio_pin_array[i]);
    }

    // Animation set up
    uint16_t fft_band_energies[NUM_DISTINCT_BARS];
    uint8_t current_heights[NUM_DISTINCT_BARS] = {0};
    uint8_t display_heights [TOTAL_VIS_BARS] = {0};
    uint32_t animation_frame[TOTAL_VIS_BARS][VIS_BAR_HEIGHT];
    uint32_t a_frame_normalized[NUM_STRIPS][NUM_PIXELS];

    while (true) {
        uint32_t color = current_color;
        if (new_data_ready) {
            for (int i = 0; i < NUM_DISTINCT_BARS; i++) {
                fft_band_energies[i] = read_buffer[i];
            }
        }
        
        uint8_t new_heights[NUM_DISTINCT_BARS] = {0};
        normalize_band_energy_to_frame_height(fft_band_energies, new_heights, MAX_BAND_ENERGY);
        update_frame_heights(new_heights, current_heights, 1);

        // Right side (0–16)
        for (int i = 0; i < 18; i++) {
            display_heights[i] = new_heights[i];
        }

        // Middle gap (17)
        display_heights[17] = 0;

        // Right side (mirror: 18–34)
        for (int i = 0; i < 17; i++) {
            display_heights[18 + i] = new_heights[16 - i];
        }

        build_animation_frame(display_heights, animation_frame, urgb_u32(0x00, 0x14, 0x00));
        rotate_landscape_to_portrait(animation_frame, a_frame_normalized);
        draw_visualizer_frame_new(pio_array, sm_array, a_frame_normalized);
        
        sleep_ms(5);
    }
}

int main() {
    stdio_init_all();
    light_onboard_led();
    sleep_ms(2000);

    multicore_launch_core1(sampling_core);
    visualizer_landscape();

    return 0;
}
