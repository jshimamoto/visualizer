#include <stdint.h>
#include "utils/mic_tools.h"

// Audio settings
#define AMBIENT_THRESHOLD   1000
#define MAX_DIFF            2000

uint16_t read_mic() {
    return adc_read();
}

uint16_t get_baseline_mic_input(uint16_t NUM_SAMPLES) {
    uint32_t baseline_total = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        baseline_total += read_mic();
        sleep_us(100);
    }
    return (uint16_t)(baseline_total / NUM_SAMPLES);
}

uint16_t get_mic_output_filtered(uint16_t baseline_audio) {
    uint16_t mic_input = read_mic();
    int16_t diff = mic_input - baseline_audio;
    uint16_t mic_filtered = diff > AMBIENT_THRESHOLD ? diff : 0;
    if (mic_filtered > MAX_DIFF) mic_filtered = MAX_DIFF;

    return mic_filtered;
}

uint16_t get_pwm_brightness(uint16_t mic_filtered) {
    float scaled = logf((float)mic_filtered + 1) / logf((float)MAX_DIFF + 1);
    uint16_t brightness = (uint16_t)(scaled * UINT16_MAX);
}
