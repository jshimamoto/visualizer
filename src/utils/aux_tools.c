#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "utils/aux_tools.h"

#define MAX_DIFF 2000

uint16_t get_aux_input_diff(uint16_t baseline_audio_val) {
    int32_t diff = (int32_t)adc_read() - (int32_t)baseline_audio_val;

    if (diff < 0) diff = -diff;   // absolute value

    if (diff > MAX_DIFF) diff = MAX_DIFF;

    return (uint16_t)diff;
}