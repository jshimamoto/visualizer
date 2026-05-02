#ifndef LED_ANIMATIONs_H
#define LED_ANIMATIONs_H

#include "utils/ws2812_tools.h"

void animate_single_color();

void animate_avg_energy_intensity_color(
    uint8_t *current_frame_heights, 
    uint32_t animation_frame[TOTAL_VIS_BARS][VIS_BAR_HEIGHT], 
    uint16_t avg_energy); 
    
uint32_t get_color(uint16_t avg_energy);

void animate_bar_height_color();

#endif
