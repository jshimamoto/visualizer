/**
 * LEDs per strip: 35
 * Max hex sum per LED: 21 (21.25) => round to 20
 * Total current per LED: 5mA
 * Total current per strip max: 175mA
 * Total current among all strips: 1.4A
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "prototypes/ws2812_test.h"
#include "ws2812_test.pio.h"
#include "utils/led_tools.h"

#define IS_RGBW false
#define NUM_PIXELS 35
#define MAX_HEX 20

#define WS2812_PIN 10

const uint32_t red = ((uint32_t)(0x14) << 8 | (uint32_t)(0x00) << 16 | (uint32_t)(0x00));
const uint32_t green = ((uint32_t)(0x00) << 8 | (uint32_t)(0x14) << 16 | (uint32_t)(0x00));
const uint32_t blue = ((uint32_t)(0x00) << 8 | (uint32_t)(0x00) << 16 | (uint32_t)(0x14));

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
        ((uint32_t) (r) << 8) |
        ((uint32_t) (g) << 16) |
        (uint32_t) (b);
}


static inline void fade_from_to(uint32_t from_color, uint32_t to_color, uint8_t num_steps, PIO pio, uint sm) {
    for (int i = 0; i < num_steps; i++) {
        uint8_t r = (from_color >> 8) & 0xFF;
        uint8_t g = (from_color >> 16) & 0xFF;
        uint8_t b = from_color & 0xFF;

        for (int j = 0; j < NUM_PIXELS; j++) {
            put_pixel(pio, sm, urgb_u32(r, g, b));
        }

        sleep_ms(20);
    }
}


void ws2812_test() {
    stdio_init_all();
    light_onboard_led();

    PIO pio;
    uint sm;
    uint offset;

    // This will find a free pio and state machine for our program and load it for us
    // We use pio_claim_free_sm_and_add_program_for_gpio_range (for_gpio_range variant)
    // so we will get a PIO instance suitable for addressing gpios >= 32 if needed and supported by the hardware
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, WS2812_PIN, 1, true);
    hard_assert(success);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    while (1) {
        // Light all 3 red
        for (int i = 0; i < NUM_PIXELS; i++) {
            put_pixel(pio, sm, urgb_u32(0x40, 0x00, 0x00)); // red
        }
        sleep_ms(1000);

        // Light all 3 green
        for (int i = 0; i < NUM_PIXELS; i++) {
            put_pixel(pio, sm, urgb_u32(0x00, 0x10, 0x00)); // green
        }
        sleep_ms(1000);

        // Light all 3 blue
        for (int i = 0; i < NUM_PIXELS; i++) {
            put_pixel(pio, sm, urgb_u32(0x00, 0xF0, 0x04)); // blue
        }
        sleep_ms(1000);

        // Turn them off
        for (int i = 0; i < NUM_PIXELS; i++) {
            put_pixel(pio, sm, 0x00000000); // off
        }
        sleep_ms(1000);
    }

    // This will free resources and unload our program
    pio_remove_program_and_unclaim_sm(&ws2812_program, pio, sm, offset);
}
