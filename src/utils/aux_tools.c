#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "utils/aux_tools.h"

#define MAX_DIFF 2000

uint16_t get_aux_input_diff(uint16_t baseline_audio_val) {
    uint16_t aux_input = adc_read();
    uint16_t aux_diff = aux_input - baseline_audio_val;
    if (aux_diff < 0) aux_diff = 0;
    if (aux_diff > MAX_DIFF) aux_diff = MAX_DIFF;
    return aux_diff;
}