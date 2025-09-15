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
#include "prototypes/visualizer_4_strip.h"
#include "utils/led_tools.h"
#include "utils/ws2812_tools.h"

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
    
    for (int i = 0; i < NUM_STRIPS; i++) {
        pio_set_sm_and_init_ws2812_program(&pio, &sm_array[i], &offset_array[i], gpio_pin_array[i]);
    }

    // Animation set up
    uint32_t color = urgb_u32(0x02, 0x10, 0x02);
    uint8_t current_heights[NUM_STRIPS] = {0};

    while (true) {
        for (int frame = 0; frame < 50; frame++) {
            update_energy_heights(band_energies[frame], current_heights, 1);
            draw_visualizer_frame(pio, sm_array, current_heights, color);
            sleep_ms(25);
        }
    }
}
