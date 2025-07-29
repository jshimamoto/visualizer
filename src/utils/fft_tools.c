#include <stdint.h>
#include "kiss_fftr.h"
#include "utils/fft_tools.h"
#include "pico/time.h"

kiss_fftr_cfg fft_cfg;
int16_t adc_buffer[FFT_SIZE];
kiss_fft_cpx fft_out[FFT_SIZE / 2 + 1];

// Group bins logarithmically
static const int fft_5_band_ranges[5][2] = {
    {0, 5}, {6, 12}, {13, 25}, {26, 51}, {52, 64}
};

void set_fft_band_energies(uint16_t band_energies[NUM_BANDS]) {
    if (!fft_cfg) {
        fft_cfg = kiss_fftr_alloc(FFT_SIZE, 0, 0, 0);
    }

    // Fill adc_buffer with time-domain samples
    // NOTE: Assumes adc_init() and adc_select_input() were already called from parent
    absolute_time_t next_time = get_absolute_time();
    for (int i = 0; i < FFT_SIZE; ++i) {
        adc_buffer[i] = adc_read();
        next_time = delayed_by_us(next_time, 100);
        sleep_until(next_time);
    }

    // Run FFT
    kiss_fftr(fft_cfg, adc_buffer, fft_out);

    // Zero the output array
    for (int i = 0; i < NUM_BANDS; ++i) {
        band_energies[i] = 0;
    }

    // Sum magnitudes in each band
    for (int band = 0; band < NUM_BANDS; ++band) {
        int start = (fft_5_band_ranges)[band][0];
        int end = (fft_5_band_ranges)[band][1];

        uint32_t sum = 0;
        for (int bin = start; bin <= end && bin < FFT_SIZE / 2 + 1; ++bin) {
            int real = fft_out[bin].r;
            int imag = fft_out[bin].i;
            sum += real * real + imag * imag;  // power = magnitude squared
        }

        // Optionally apply log or sqrt compression here
        band_energies[band] = (uint16_t)sqrtf((float)sum);
    }
}

uint16_t get_pwm_brightness_from_energy(uint32_t band_energy) {
    if (band_energy > MAX_BAND_ENERGY) {
        band_energy = MAX_BAND_ENERGY;
    }

    float scaled = logf((float)band_energy + 1.0f) / logf((float)MAX_BAND_ENERGY + 1.0f);
    return (uint16_t)(scaled * UINT16_MAX);
}
