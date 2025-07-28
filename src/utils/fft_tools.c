#include <stdint.h>
#include "kiss_fftr.h"

// FFT settings
#define SAMPLE_RATE         10000
#define FFT_SIZE            128
#define NUM_BANDS           5

kiss_fftr_cfg fft_cfg;
int16_t adc_buffer[FFT_SIZE];
kiss_fft_cpx fft_out[FFT_SIZE / 2 + 1];

// Group bins logarithmically
static const int fft_5_band_ranges[5][2] = {
    {0, 5}, {6, 12}, {13, 25}, {26, 51}, {52, 64}
};

void get_fft_band_energies(uint16_t band_energies[NUM_BANDS], int16_t adc_buffer[FFT_SIZE]) {
    // Fill adc_buffer with time-domain samples
    for (int i = 0; i < FFT_SIZE; ++i) {
        adc_buffer[i] = adc_read();
        sleep_us(1000000 / SAMPLE_RATE);
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
