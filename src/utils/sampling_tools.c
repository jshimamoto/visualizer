#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "utils/sampling_tools.h"
#include "utils/fft_tools.h"
#include "utils/ws2812_tools.h"
#include "utils/aux_tools.h"
#include "utils/ws2812_config.h"

// Sampling buffers
uint16_t fft_band_energies_A[NUM_DISTINCT_BARS];
uint16_t fft_band_energies_B[NUM_DISTINCT_BARS];

uint16_t *write_buffer;
uint16_t *read_buffer;

void init_buffers(void) {
    write_buffer = fft_band_energies_A;
    read_buffer  = fft_band_energies_B;
}

volatile bool new_data_ready = false;

void sampling_core() {
    uint16_t baseline_audio_val = AUX_SIGNAL_BASELINE;
    int16_t fft_band_energies[NUM_DISTINCT_BARS];

    while (true) {
        set_fft_band_energies(write_buffer, NUM_DISTINCT_BARS, baseline_audio_val, INPUT_MODE);

        // swap buffers safely
        uint16_t *temp = write_buffer;
        write_buffer = read_buffer;
        read_buffer = temp;

        new_data_ready = true;
    }
}