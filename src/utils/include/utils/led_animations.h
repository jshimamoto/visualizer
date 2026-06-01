#ifndef LED_ANIMATIONs_H
#define LED_ANIMATIONs_H

#include "utils/ws2812_tools.h"

typedef struct {
    int r;
    int g;
    int b;
} rgb_t;

typedef struct {
    int start;
    int end;
    rgb_t color;
} BarColorRange;

void animate_single_color();

// ============================================================
#define NUM_COLORS 6
extern rgb_t color_cycle[];
void animate_fading_color(int *color_index, int *fade_step, uint32_t *current_color);

// ============================================================
extern BarColorRange bar_colors[];
void animate_static_color_bars(
    uint8_t *current_frame_heights, 
    uint32_t animation_frame[TOTAL_VIS_BARS][VIS_BAR_HEIGHT]
);

// ============================================================
void animate_avg_energy_intensity_color(
    uint8_t *current_frame_heights, 
    uint32_t animation_frame[TOTAL_VIS_BARS][VIS_BAR_HEIGHT], 
    uint16_t avg_energy
); 

// ============================================================
void animate_bar_height_color(
    uint8_t *current_frame_heights, 
    uint32_t animation_frame[TOTAL_VIS_BARS][VIS_BAR_HEIGHT]
);
    
uint32_t get_color(uint16_t avg_energy);


#endif
