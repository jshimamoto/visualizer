#include "utils/led_animations.h"
#include "utils/ws2812_tools.h"
#include "utils/ws2812_config.h"
#include "utils/fft_tools.h"
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"

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

/*
Animate with static color for groups of bars or individual
*/
BarColorRange bar_colors[] = {
    {0,  0,  {0x00, 0x14, 0x00}}, // green
    {1,  1,  {0x00, 0x12, 0x02}},
    {2,  2,  {0x00, 0x10, 0x04}},
    {3,  3,  {0x00, 0x0E, 0x06}},
    {4,  4,  {0x00, 0x0C, 0x08}},
    {5,  5,  {0x00, 0x0A, 0x0A}}, // cyan

    {6,  6,  {0x00, 0x08, 0x0C}},
    {7,  7,  {0x00, 0x06, 0x0E}},
    {8,  8,  {0x00, 0x04, 0x10}},
    {9,  9,  {0x00, 0x02, 0x12}},
    {10, 10, {0x00, 0x00, 0x14}}, // blue

    {11, 11, {0x02, 0x00, 0x12}},
    {12, 12, {0x04, 0x00, 0x10}},
    {13, 13, {0x06, 0x00, 0x0E}},
    {14, 14, {0x08, 0x00, 0x0C}},
    {15, 15, {0x0A, 0x00, 0x0A}}, // magenta

    {16, 16, {0x0C, 0x00, 0x08}},
    {17, 17, {0x0E, 0x00, 0x06}},
};
int mirror_bar(int col) {
    if (col < 18) return col;
    return 34 - col;
}
rgb_t get_bar_color(int col) {
    col = mirror_bar(col);

    for (int i = 0; i < sizeof(bar_colors)/sizeof(bar_colors[0]); i++) {
        if (col >= bar_colors[i].start && col <= bar_colors[i].end) {
            return bar_colors[i].color;
        }
    }

    return (rgb_t){0x00, 0x00, 0x00}; // fallback
}
void animate_static_color_bars(uint8_t *current_frame_heights, uint32_t animation_frame[TOTAL_VIS_BARS][VIS_BAR_HEIGHT]) {
    for (int col = 0; col < TOTAL_VIS_BARS; col++) {
        rgb_t color = get_bar_color(col);
        for (int bar_pixel = 0; bar_pixel < VIS_BAR_HEIGHT; bar_pixel++) {
            if (bar_pixel < current_frame_heights[col]) {
                animation_frame[col][bar_pixel] = urgb_u32(color.r, color.g, color.b);
            } else {
                animation_frame[col][bar_pixel] = urgb_u32(0x00, 0x00, 0x01);
            }
        }
    }
}

// ===============================================================================================================

/*
Animate with fading color for all bars
*/
rgb_t color_cycle[] = {
    {0x14, 0x00, 0x00},
    {0x00, 0x14, 0x00},
    {0x00, 0x00, 0x14},
    {0x0A, 0x0A, 0x00},
    {0x0A, 0x00, 0x0A},
    {0x00, 0x0A, 0x0A}
};
void fade_color(int *color_index, int *fade_step, uint32_t *current_color) {
    rgb_t from = color_cycle[*color_index];
    rgb_t to = color_cycle[(*color_index + 1) % NUM_COLORS];

    int r = (from.r + ((to.r - from.r) * *fade_step) / FADE_STEPS);
    int g = from.g + ((to.g - from.g) * *fade_step) / FADE_STEPS;
    int b = from.b + ((to.b - from.b) * *fade_step) / FADE_STEPS;

    *current_color = urgb_u32(r, g, b);
    (*fade_step)++;

    if (*fade_step >= FADE_STEPS) {
        *fade_step = 0;
        *color_index = (*color_index + 1) % NUM_COLORS;
    }
}

// ===============================================================================================================

BarColorRange bar_colors_vertical[] = {
    {0,  1,  {0x00, 0x14, 0x00}}, // green
    {2,  4,  {0x0B, 0x08, 0x00}}, // orange
    {5,  7,  {0x0E, 0x00, 0x06}} // magenta
};
/*
Animate with certain colors for certain heights
*/
void animate_bar_height_color(uint8_t *current_frame_heights, uint32_t animation_frame[TOTAL_VIS_BARS][VIS_BAR_HEIGHT]) {
    for (int col = 0; col < TOTAL_VIS_BARS; col++) {
        for (int bar_pixel = 0; bar_pixel < VIS_BAR_HEIGHT; bar_pixel++) {
            if (bar_pixel < current_frame_heights[col]) {
                if (bar_pixel < 2) {
                    animation_frame[col][bar_pixel] = urgb_u32(0x00, 0x00, 0x14); // green
                } else if (bar_pixel >= 2 && bar_pixel <= 4) {
                    animation_frame[col][bar_pixel] = urgb_u32(0x00, 0x0A, 0x0A); // cyan
                } else if (bar_pixel >= 5) {
                    animation_frame[col][bar_pixel] = urgb_u32(0x00, 0x14, 0x00); // blue
                }
            } else {
                animation_frame[col][bar_pixel] = urgb_u32(0x00, 0x00, 0x01);
            }
        }
    }
}


// ===============================================================================================================

/*
Animate visualizer based off of average band energy
low -> blue
high -> green
Note: this one sucks as is
*/
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

// ===============================================================================================================

static const StartupFrame startup_animation_frames[] = {
    {.height_array = {0}, .color = {0}, 5},
    {.height_array = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, .color = {0, 20, 0}, .num_frames = 10},
    {.height_array = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0}, .color = {0, 20, 0}, .num_frames = 10},
    {.height_array = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1, 1, 0}, .color = {0, 20, 0}, .num_frames = 10},
    {.height_array = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 2, 2, 1, 1, 0}, .color = {0, 20, 0}, .num_frames = 10},
    {.height_array = {5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0}, .color = {0, 20, 0}, .num_frames = 10},
    {.height_array = {6, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0}, .color = {0, 20, 0}, .num_frames = 10},
    {.height_array = {7, 7, 7, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0}, .color = {0, 20, 0}, .num_frames = 10},
    {.height_array = {8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0}, .color = {0, 20, 0}, .num_frames = 10},
};
size_t num_frames = sizeof(startup_animation_frames) / sizeof(startup_animation_frames[0]);

void startup_animation(
    PIO pio_instance, 
    uint *sm_array,
    uint32_t animation_frame[TOTAL_VIS_BARS][VIS_BAR_HEIGHT],
    uint32_t a_frame_normalized[NUM_STRIPS][NUM_PIXELS],
    uint32_t a_frame_snakified[NUM_CHAINS][NUM_PIXELS_IN_CHAIN]
) {
    uint8_t display_heights [TOTAL_VIS_BARS] = {0};

    for (int frame = 0; frame < num_frames; frame++) {
        for (int j = 0; j < startup_animation_frames[frame].num_frames; j++){
            // Right side (0–17)
            for (int bar = 0; bar < 18; bar++) {
                display_heights[bar] = startup_animation_frames[frame].height_array[bar];
            }

            // Right side (mirror: 18–34)
            for (int bar = 0; bar < 17; bar++) {
                display_heights[18 + bar] = startup_animation_frames[frame].height_array[16 - bar];
            }

            uint32_t current_color = urgb_u32(
                startup_animation_frames[frame].color.r, 
                startup_animation_frames[frame].color.g,
                startup_animation_frames[frame].color.b
            );
            animate_single_color(display_heights, animation_frame, current_color);

            // Rendering
            transpose_matrix(animation_frame, a_frame_normalized);
            snakify_animation_frame(a_frame_normalized, a_frame_snakified);
            draw_visualizer_frame_matrix_snake(pio_instance, sm_array, a_frame_snakified);
            
            sleep_ms(10);
        }
    }

    memset(animation_frame, 0, TOTAL_VIS_BARS * VIS_BAR_HEIGHT * sizeof(uint32_t));
}