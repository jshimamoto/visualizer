/**
 * LEDs per strip: 35
 * Max hex sum per LED: 21 (21.25) => round to 20
 * Total current per LED: 5mA
 * Total current per strip max: 175mA
 * Total current among all (8) strips: 1.4A
 */

// Standard C imports
#include <stdio.h>
#include <stdlib.h>

// Pico SDK
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"

// Utils
#include "utils/fft_tools.h"
#include "utils/mic_tools.h"
#include "utils/led_tools.h"
#include "utils/ws2812_tools.h"

// File header
#include "prototypes/visualizer_4_strip.h"

void visualizer_4_strip() {
    stdio_init_all();

    // ADC init for mic input
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_INPUT);

    // PIO and GPIO set up
    PIO pio;
    uint gpio_pin_array[NUM_STRIPS] = WS2812_PINS; 
    uint sm_array[NUM_STRIPS];
    uint offset_array[NUM_STRIPS];

    // Set baseline audio input
    uint16_t baseline_audio_val = get_baseline_mic_input(BASELINE_SAMPLES);
    float smoothed_band_energies[NUM_STRIPS] = {0};
    
    for (int i = 0; i < NUM_STRIPS; i++) {
        pio_set_sm_and_init_ws2812_program(&pio, &sm_array[i], &offset_array[i], gpio_pin_array[i]);
    }

    // Animation set up
    uint32_t color = urgb_u32(0x02, 0x10, 0x02);
    uint8_t current_heights[NUM_STRIPS] = {0};

    while (true) {
        // Filter out ambient noise
        uint16_t mic_filtered = get_mic_output_filtered(baseline_audio_val);

        // Initialize band energy array
        uint16_t fft_band_energies[NUM_STRIPS];
        set_fft_band_energies(fft_band_energies, NUM_STRIPS, baseline_audio_val);
        update_energy_heights_fft(fft_band_energies, current_heights, 1);
        // print_band_energies_8bit((current_heights), NUM_STRIPS);

        // Draw visualizer
        // update_energy_heights(fft_band_energies, current_heights, 1);
        draw_visualizer_frame(pio, sm_array, current_heights, color);
        sleep_ms(25);
    }
}
