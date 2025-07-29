#ifndef FFT_TOOLS_H
#define FFT_TOOLS_H

// FFT settings
#define SAMPLE_RATE         10000
#define FFT_SIZE            128
#define NUM_BANDS           5
#define MAX_BAND_ENERGY     3000000

static const int fft_5_band_ranges[5][2];

// NOTE: Assumes adc_init() and adc_select_input() were already called
void set_fft_band_energies(uint16_t band_energies[NUM_BANDS]);

uint16_t get_pwm_brightness_from_energy(uint32_t band_energy);

#endif
