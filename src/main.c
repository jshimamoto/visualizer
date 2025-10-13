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


void visualizer_8_strip() {
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

    // Set baseline audio input
    uint16_t baseline_audio_val = get_baseline_mic_input(BASELINE_SAMPLES);
    
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
    uint8_t current_heights[NUM_STRIPS] = {0};

    while (true) {
        // Initialize band energy array
        uint16_t fft_band_energies[NUM_STRIPS];
        set_fft_band_energies(fft_band_energies, NUM_STRIPS, baseline_audio_val);
        update_energy_heights_fft(fft_band_energies, current_heights, 1);

        // Draw visualizer
        draw_visualizer_frame(pio_array, sm_array, current_heights, current_color);
        sleep_ms(10);
    }
}

int main() {
    stdio_init_all();
    light_onboard_led();
    sleep_ms(1000);

    multicore_launch_core1(change_color_core);
    visualizer_8_strip();

    return 0;
}
