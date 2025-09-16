#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "utils/mic_tools.h"
#include "hardware/adc.h"

// Audio settings
#define AMBIENT_THRESHOLD   500
#define MAX_DIFF            2000

uint16_t get_baseline_mic_input(uint16_t NUM_SAMPLES) {
    uint32_t baseline_total = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        baseline_total += adc_read();
        sleep_ms(1);
    }
    printf("Baseline mic value: %d\n", (baseline_total / NUM_SAMPLES));
    return (uint16_t)(baseline_total / NUM_SAMPLES);
}

uint16_t get_mic_output_filtered(uint16_t baseline_audio_val) {
    uint16_t mic_input = adc_read();
    int16_t diff = mic_input - baseline_audio_val;
    uint16_t mic_filtered = diff > AMBIENT_THRESHOLD ? diff : 0;
    if (mic_filtered < 0) mic_filtered = 0;
    if (mic_filtered > MAX_DIFF) mic_filtered = MAX_DIFF;

    return mic_filtered;
}

uint16_t get_pwm_brightness_from_mic(uint16_t mic_filtered) {
    float scaled = logf((float)mic_filtered + 1) / logf((float)MAX_DIFF + 1);
    return (uint16_t)(scaled * UINT16_MAX);
}
