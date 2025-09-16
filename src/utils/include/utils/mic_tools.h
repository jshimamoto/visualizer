#ifndef MIC_TOOLS_H
#define MIC_TOOLS_H

// Mic component used: 

// Mic Input
#define BASELINE_SAMPLES 5000

uint16_t get_baseline_mic_input(uint16_t NUM_SAMPLES);

uint16_t read_mic();

uint16_t get_mic_output_filtered(uint16_t baseline_audio);

uint16_t get_pwm_brightness_from_mic(uint16_t mic_filtered);

#endif
