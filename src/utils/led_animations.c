#include "utils/led_animations.h"
#include "utils/ws2812_tools.h"
#include "utils/fft_tools.h"

/*
Animate visualizer with a single input color
*/
void animate_single_color(uint8_t *current_frame_heights, uint32_t animation_frame[TOTAL_VIS_BARS][VIS_BAR_HEIGHT], uint32_t color) {
    for (int col = 0; col < TOTAL_VIS_BARS; col++) {
        for (int bar_pixel = 0; bar_pixel < VIS_BAR_HEIGHT; bar_pixel++) {
            if (bar_pixel < current_frame_heights[col]) {
                animation_frame[col][bar_pixel] = color;
            } else {
                animation_frame[col][bar_pixel] = urgb_u32(0x00, 0x00, 0x01);
            }
        }
    }
}


// ===============================================================================================================

// const uint32_t color_cycle[] = {
//     urgb_u32(0x14, 0x00, 0x00), // red
//     urgb_u32(0x00, 0x14, 0x00), // green
//     urgb_u32(0x00, 0x00, 0x14), // blue
//     urgb_u32(0x0A, 0x0A, 0x00), // yellow
//     urgb_u32(0x0A, 0x00, 0x0A), // magenta
//     urgb_u32(0x00, 0x0A, 0x0A)  // cyan
// };

uint32_t get_color(uint16_t avg_energy) {
    float t = avg_energy / 3000;

    if (t > 1.0f) t = 1.0f;
    if (t < 0.0f) t = 0.0f;

    uint8_t r = 0;
    uint8_t g = (uint8_t)(10 + t * (20 - 10));
    uint8_t b = (uint8_t)(10 + t * (0  - 10));

    // pack into 0xRRGGBB
    return urgb_u32(0x00, g, b);
}

/*
Animate visualizer based off of average band energy
low -> blue
high -> green
*/
void animate_avg_energy_intensity_color(
    uint8_t *current_frame_heights, 
    uint32_t animation_frame[TOTAL_VIS_BARS][VIS_BAR_HEIGHT], 
    uint16_t avg_energy) 
{
    uint32_t color = get_color(avg_energy);
    for (int col = 0; col < TOTAL_VIS_BARS; col++) {
        for (int bar_pixel = 0; bar_pixel < VIS_BAR_HEIGHT; bar_pixel++) {
            if (bar_pixel < current_frame_heights[col]) {
                animation_frame[col][bar_pixel] = color;
            } else {
                animation_frame[col][bar_pixel] = urgb_u32(0x00, 0x00, 0x01);
            }
        }
    }
}

void animate_bar_height_color();