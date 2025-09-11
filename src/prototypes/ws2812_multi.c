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
#include "prototypes/ws2812_multi.h"
#include "ws2812.pio.h"
#include "utils/led_tools.h"

#define IS_RGBW false
#define NUM_PIXELS 35
#define NUM_STRIPS 2
#define MAX_HEX 0x14

#define WS2812_PINS {10, 11}
#define WS2812_PIN_1 10
#define WS2812_PIN_2 11

#define FADE_STEPS 500

const uint32_t red = ((uint32_t)(MAX_HEX) << 8 | (uint32_t)(0x00) << 16 | (uint32_t)(0x00));
const uint32_t green = ((uint32_t)(0x00) << 8 | (uint32_t)(MAX_HEX) << 16 | (uint32_t)(0x00));
const uint32_t blue = ((uint32_t)(0x00) << 8 | (uint32_t)(0x00) << 16 | (uint32_t)(MAX_HEX));

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
        ((uint32_t) (r) << 8) |
        ((uint32_t) (g) << 16) |
        (uint32_t) (b);
}

static inline void fade_from_to(uint32_t from_color, uint32_t to_color, PIO pio, uint sm) {
    for (int i = 0; i < FADE_STEPS; i++) {
        uint8_t r_from = (from_color >> 8) & 0xFF;
        uint8_t g_from = (from_color >> 16) & 0xFF;
        uint8_t b_from = from_color & 0xFF;

        uint8_t r_to = (to_color >> 8) & 0xFF;
        uint8_t g_to = (to_color >> 16) & 0xFF;
        uint8_t b_to = to_color & 0xFF;

        uint8_t r = r_from + ((r_to - r_from) * i / FADE_STEPS);
        uint8_t g = g_from + ((g_to - g_from) * i / FADE_STEPS);
        uint8_t b = b_from + ((b_to - b_from) * i / FADE_STEPS);

        for (int j = 0; j < NUM_PIXELS; j++) {
            put_pixel(pio, sm, urgb_u32(r, g, b));
        }

        sleep_ms(1);
    }
}

// void pio_set_sm_and_init_ws2812_program(PIO pio, uint sm, uint offset, uint8_t gpio_pin) {
//     bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, gpio_pin, 1, true);
//     hard_assert(success);
//     hard_assert(sm != -1);
//     ws2812_program_init(pio, sm, offset, gpio_pin, 800000, IS_RGBW);
// }

void ws2812_multi() {
    stdio_init_all();
    light_onboard_led();

    PIO pio;
    uint sm_array[NUM_STRIPS];
    uint offset;

    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm_array[0], &offset, WS2812_PIN_1, 1, true);
    hard_assert(success);

    sm_array[1] = pio_claim_unused_sm(pio, true);
    hard_assert(sm_array[1] != -1);

    ws2812_program_init(pio, sm_array[0], offset, WS2812_PIN_1, 800000, IS_RGBW);
    ws2812_program_init(pio, sm_array[1], offset, WS2812_PIN_2, 800000, IS_RGBW);

    while (true) {
        fade_from_to(red, green, pio, sm_array);
        fade_from_to(red, green, pio, sm_array);

        fade_from_to(green, blue, pio, sm_array);
        fade_from_to(green, blue, pio, sm_array);

        fade_from_to(blue, red, pio, sm_array);
        fade_from_to(blue, red, pio, sm_array);
    }
}
