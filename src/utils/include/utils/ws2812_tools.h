#ifndef WS2812_TOOLS_H
#define WS2812_TOOLS_H

#include "hardware/pio.h"
#include "ws2812_config.h"

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
        ((uint32_t) (r) << 8) |
        ((uint32_t) (g) << 16) |
        (uint32_t) (b);
}

void pio_set_sm_and_init_ws2812_program(PIO *pio, uint *sm, uint *offset, uint8_t gpio_pin);

void fade_from_to(uint32_t from_color, uint32_t to_color, PIO pio, uint *sm_array);

#endif
