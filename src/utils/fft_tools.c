#include <stdint.h>
#include <stdio.h>
#include "hardware/adc.h"
#include "pico/time.h"
#include "kiss_fftr.h"
#include "utils/fft_tools.h"
#include "utils/mic_tools.h"
#include "utils/aux_tools.h"

kiss_fftr_cfg fft_cfg;
float adc_buffer[FFT_SIZE];
kiss_fft_cpx fft_out[FFT_SIZE / 2 + 1];

// Group bins logarithmically
static const int fft_4_band_ranges[4][2] = {
    {0, 7},   // Band 1: Bass (covers 0–7)
    {8, 19},  // Band 2: Low-Mids (covers 8–19)
    {20, 39}, // Band 3: High-Mids (covers 20–39)
    {40, 64}  // Band 4: Treble (covers 40–64)
};

static const int fft_8_band_ranges[8][2] = {
    {0, 1},     // Band 1: Sub-bass
    {2, 3},     // Band 2: Bass
    {4, 7},     // Band 3: Low-Mid
    {8, 11},    // Band 4: Mid
    {12, 19},   // Band 5: Upper-Mid
    {20, 31},   // Band 6: Presence
    {32, 47},   // Band 7: Brilliance
    {48, 64}    // Band 8: Air
};

void set_fft_band_energies(uint16_t *band_energies, int num_bands, uint16_t baseline_audio_val, char input_mode[]) {
    if (!fft_cfg) {
        fft_cfg = kiss_fftr_alloc(FFT_SIZE, 0, 0, 0);
    }

    // Fill adc_buffer with time-domain samples
    // NOTE: Assumes adc_init() and adc_select_input() were already called from parent
    absolute_time_t next_time = get_absolute_time();
    for (int i = 0; i < FFT_SIZE; ++i) {
        if (strcmp(input_mode, "MIC")) {
            adc_buffer[i] = get_mic_output_filtered(baseline_audio_val);
            next_time = delayed_by_us(next_time, 100);
            sleep_until(next_time);
        } else if (strcmp(input_mode, "AUX"))
        {
            adc_buffer[i] = get_aux_input_diff(baseline_audio_val);
            next_time = delayed_by_us(next_time, 100);
            sleep_until(next_time);
        }
    }

    // Run FFT
    kiss_fftr(fft_cfg, adc_buffer, fft_out);

    // Zero the output array
    for (int i = 0; i < num_bands; ++i) {
        band_energies[i] = 0;
    }

    // Sum magnitudes in each band
    for (int band = 0; band < num_bands; ++band) {
        int start = (fft_8_band_ranges)[band][0];
        int end = (fft_8_band_ranges)[band][1];

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

void print_band_energies_8bit(uint8_t *band_energies, uint8_t num_bands) {
    printf("Band energies: [");
    for (int i = 0; i < num_bands; i++) {
        if (i != num_bands - 1) {
            printf("%d, ", band_energies[i]);
        } else {
            printf("%d]\n", band_energies[i]);
        }
    }
}

void print_band_energies_16bit(uint16_t *band_energies, uint8_t num_bands) {
    printf("Band energies: [");
    for (int i = 0; i < num_bands; i++) {
        if (i != num_bands - 1) {
            printf("%d, ", band_energies[i]);
        } else {
            printf("%d]\n", band_energies[i]);
        }
    }
}

uint16_t get_pwm_brightness_from_energy(uint32_t band_energy) {
    if (band_energy > MAX_BAND_ENERGY) {
        band_energy = MAX_BAND_ENERGY;
    }

    float scaled = logf((float)band_energy + 1.0f) / logf((float)MAX_BAND_ENERGY + 1.0f);
    return (uint16_t)(scaled * UINT16_MAX);
}
