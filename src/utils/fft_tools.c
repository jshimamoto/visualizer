#include <stdint.h>
#include <stdio.h>
#include "hardware/adc.h"
#include "pico/time.h"
#include "kiss_fftr.h"
#include "utils/fft_tools.h"
#include "utils/mic_tools.h"
#include "utils/aux_tools.h"
#include "utils/ws2812_config.h"

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

// static const int fft_8_band_ranges[8][2] = {
//     {0, 1},     // Band 1: Sub-bass
//     {2, 3},     // Band 2: Bass
//     {4, 7},     // Band 3: Low-Mid
//     {8, 11},    // Band 4: Mid
//     {12, 19},   // Band 5: Upper-Mid
//     {20, 31},   // Band 6: Presence
//     {32, 47},   // Band 7: Brilliance
//     {48, 64}    // Band 8: Air
// };

static const int fft_8_band_ranges[8][2] = {
    {2, 3},
    {4, 6},
    {7, 10},
    {11, 16},
    {17, 26},
    {27, 40},
    {41, 56},
    {57, 64}
};

static const float _8_band_gain[NUM_STRIPS] = {
    0.7f,   // bass (reduce)
    0.9f,
    1.1f,
    1.4f,
    1.6f,
    1.9f,
    2.1f,
    2.3f    // highs (boost)
};

static const int fft_17_band_ranges[17][2] = {
    {1, 1}, 
    {2, 3},
    {4, 5},
    {6, 7},
    {8, 9},
    {10, 11},
    {12, 13},
    {14, 16},
    {17, 20},
    {21, 24},
    {25, 28},
    {29, 32},
    {33, 36},
    {37, 40},
    {41, 46},
    {47, 53},
    {54, 64}
};

float _17_band_gain[17] = {
    0.7f,  // bass slightly reduced
    0.75f,
    0.8f,
    0.9f,
    1.0f,
    1.1f,
    1.2f,
    1.3f,
    1.5f,  // mids
    1.7f,
    1.9f,
    2.1f,
    2.3f,
    2.5f,
    2.7f,
    2.9f,
    3.0f   // highs capped
};

static const int fft_18_band_ranges[18][2] = {
    {1, 1}, 
    {2, 3},
    {4, 5},
    {6, 7},
    {8, 9},
    {10, 11},
    {12, 13},
    {14, 16},
    {17, 20},
    {21, 24},
    {25, 28},
    {29, 32},
    {33, 36},
    {37, 40},
    {41, 46},
    {47, 50},
    {51, 53},
    {54, 64}
};

float _18_band_gain[18] = {
    0.7f,
    0.75f,
    0.8f,
    0.9f,
    1.0f,
    1.1f,
    1.2f,
    1.3f,
    1.45f,
    1.6f,
    1.8f,
    2.0f,
    2.2f,
    2.4f,
    2.6f,
    2.75f,
    2.9f,
    3.0f
};

// Overwrites the input array with band energies from the audio sample
// NOTE: Assumes adc_init() and adc_select_input() were already called
void set_fft_band_energies(uint16_t *band_energies, int num_bands, uint16_t baseline_audio_val, char input_mode[]) {
    if (!fft_cfg) {
        fft_cfg = kiss_fftr_alloc(FFT_SIZE, 0, 0, 0);
    }

    // Fill adc_buffer with time-domain samples
    absolute_time_t next_time = get_absolute_time();
    for (int i = 0; i < FFT_SIZE; ++i) {
        if (strcmp(input_mode, "MIC") == 0) {
            adc_buffer[i] = get_mic_output_filtered(baseline_audio_val);
        } else if (strcmp(input_mode, "AUX") == 0)
        {
            adc_buffer[i] = get_aux_input_diff(baseline_audio_val);
        }

        next_time = delayed_by_us(next_time, 100);
        sleep_until(next_time);
    }

    // Run FFT
    kiss_fftr(fft_cfg, adc_buffer, fft_out);

    // Zero the output array
    for (int i = 0; i < num_bands; ++i) {
        band_energies[i] = 0;
    }

    // Sum magnitudes in each band
    for (int band = 0; band < num_bands; ++band) {
        int start = (fft_17_band_ranges)[band][0];
        int end = (fft_17_band_ranges)[band][1];

        uint32_t sum = 0;
        for (int bin = start; bin <= end && bin < FFT_SIZE / 2 + 1; ++bin) {
            int real = fft_out[bin].r;
            int imag = fft_out[bin].i;
            sum += real * real + imag * imag;
        }

        band_energies[band] = (uint16_t)((sqrtf((float)sum)) * _17_band_gain[band]);
    }
}

// Overwrites the input array with band energies from the audio sample
// Uses a 50% FFT overlap to minimize missed beats
// NOTE: Assumes adc_init() and adc_select_input() were already called
void set_fft_band_energies_overlap(uint16_t *band_energies,
                          int num_bands,
                          uint16_t baseline_audio_val,
                          char input_mode[]) {

    static bool initialized = false;

    if (!fft_cfg) {
        fft_cfg = kiss_fftr_alloc(FFT_SIZE, 0, 0, 0);
    }

    absolute_time_t next_time = get_absolute_time();

    if (!initialized) {
        for (int i = 0; i < FFT_SIZE; ++i) {
            if (strcmp(input_mode, "MIC") == 0) {
                adc_buffer[i] = get_mic_output_filtered(baseline_audio_val);
            } else {
                adc_buffer[i] = get_aux_input_diff(baseline_audio_val);
            }

            next_time = delayed_by_us(next_time, 100);
            sleep_until(next_time);
        }
        initialized = true;
    } else {
        memmove(adc_buffer,
                adc_buffer + FFT_SIZE / 2,
                (FFT_SIZE / 2) * sizeof(float));

        for (int i = FFT_SIZE / 2; i < FFT_SIZE; ++i) {
            if (strcmp(input_mode, "MIC") == 0) {
                adc_buffer[i] = get_mic_output_filtered(baseline_audio_val);
            } else {
                adc_buffer[i] = get_aux_input_diff(baseline_audio_val);
            }

            next_time = delayed_by_us(next_time, 100);
            sleep_until(next_time);
        }
    }

    static float windowed[FFT_SIZE];

    for (int i = 0; i < FFT_SIZE; i++) {
        float w = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (FFT_SIZE - 1)));
        windowed[i] = adc_buffer[i] * w;
    }

    kiss_fftr(fft_cfg, windowed, fft_out);

    for (int i = 0; i < num_bands; ++i) {
        band_energies[i] = 0;
    }

    for (int band = 0; band < num_bands; ++band) {
        int start = fft_17_band_ranges[band][0];
        int end   = fft_17_band_ranges[band][1];

        uint32_t sum = 0;
        for (int bin = start; bin <= end && bin < FFT_SIZE / 2 + 1; ++bin) {
            int real = fft_out[bin].r;
            int imag = fft_out[bin].i;
            sum += real * real + imag * imag;
        }

        band_energies[band] = (uint16_t)(sqrtf((float)sum) * _17_band_gain[band]);
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
