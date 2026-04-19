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

#include "prototypes/visualizer_landscape.h"

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

    uint16_t baseline_audio_val = AUX_SIGNAL_BASELINE;
  
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
    uint32_t color = urgb_u32(0x02, 0x10, 0x02);
    uint8_t current_heights[NUM_STRIPS] = {0};
    uint32_t animation_frame[TOTAL_VIS_BARS][VIS_BAR_HEIGHT];
    uint32_t a_frame_normalized[NUM_STRIPS][NUM_PIXELS];

    while (true) {
        // Initialize band energy array
        uint16_t fft_band_energies[TOTAL_VIS_BARS];
        set_fft_band_energies(fft_band_energies, TOTAL_VIS_BARS, baseline_audio_val, "AUX");

        uint8_t new_heights[NUM_STRIPS] = {0};
        normalize_band_energy_to_frame_height(fft_band_energies, new_heights, MAX_BAND_ENERGY);

        build_animation_frame(new_heights, animation_frame, color);

        // Draw visualizer
        if (FRAME_ORIENTATION == 0) {
            for (int i = 0; i < NUM_STRIPS; i++) {
                for (int j = 0; j < NUM_PIXELS; j++) {
                    a_frame_normalized[i][j] = animation_frame[i][j];
                }
            }
            draw_visualizer_frame_new(pio_array, sm_array, a_frame_normalized);
        } else if (FRAME_ORIENTATION == 1) {
            rotate_landscape_to_portrait(animation_frame, a_frame_normalized);
            draw_visualizer_frame_new(pio_array, sm_array, a_frame_normalized);
        }

        sleep_ms(10);
    }
}