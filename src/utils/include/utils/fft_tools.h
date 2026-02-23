#ifndef FFT_TOOLS_H
#define FFT_TOOLS_H

// FFT settings
#define SAMPLE_RATE             10000
#define FFT_SIZE                128
#define NUM_BANDS               5
#define MAX_BAND_ENERGY         20000
#define BAND_ENERGY_THRESHOLD   500

static const int fft_5_band_ranges[5][2];

// NOTE: Assumes adc_init() and adc_select_input() were already called
void set_fft_band_energies(uint16_t *band_energies, int num_bands, uint16_t baseline_audio_val, char input_mode[]);

void print_band_energies_8bit(uint8_t *band_energies, uint8_t num_bands);
void print_band_energies_16bit(uint16_t *band_energies, uint8_t num_bands);

uint16_t get_pwm_brightness_from_energy(uint32_t band_energy);

#endif
