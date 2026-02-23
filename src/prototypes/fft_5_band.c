#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#include "kiss_fftr.h"

#include "prototypes/fft_5_band.h"
#include "utils/led_tools.h"
#include "utils/fft_tools.h"
#include "utils/mic_tools.h"

// ADC
#define ADC_INPUT 0
#define ADC_PIN 26

void main() {
    light_onboard_led(ONBOARD_LED);

    // Initialize brightness matrix
    uint16_t brightness_matrix[NUM_BANDS];
    
    // ADC init
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_INPUT);

    // Initialize and config transistor GPIO
    setup_pwm(BAND_1_TRANS_PIN);
    setup_pwm(BAND_2_TRANS_PIN);
    setup_pwm(BAND_3_TRANS_PIN);
    setup_pwm(BAND_4_TRANS_PIN);
    setup_pwm(BAND_5_TRANS_PIN);

    // Set baseline audio input
    uint16_t baseline_audio_val = get_baseline_mic_input(BASELINE_SAMPLES);

    float smoothed_band_energies[NUM_BANDS] = {0};

    while (true) {
        // Filter out ambient noise
        uint16_t mic_filtered = get_mic_output_filtered(baseline_audio_val);

        // Initialize band energy array
        uint16_t fft_band_energies[NUM_BANDS];
        set_fft_band_energies(fft_band_energies, NUM_BANDS, baseline_audio_val, "MIC");
        
        // // Set PWM brightness
        for (int i = 0; i < NUM_BANDS; i ++) {
            smoothed_band_energies[i] = 0.8f * smoothed_band_energies[i] + 0.2f * fft_band_energies[i];
            brightness_matrix[i] = get_pwm_brightness_from_energy((uint16_t)smoothed_band_energies[i]);
        }

        pwm_set_gpio_level(BAND_1_TRANS_PIN, brightness_matrix[0]);
        pwm_set_gpio_level(BAND_2_TRANS_PIN, brightness_matrix[1]);
        pwm_set_gpio_level(BAND_3_TRANS_PIN, brightness_matrix[2]);
        pwm_set_gpio_level(BAND_4_TRANS_PIN, brightness_matrix[3]);
        pwm_set_gpio_level(BAND_5_TRANS_PIN, brightness_matrix[4]);

        printf("--------------------------------------\n");
        printf("Baseline: %d, Mic: %d\n", baseline_audio_val, mic_filtered);
        printf("Band Energies: [%d, %d, %d, %d, %d]\n", 
            fft_band_energies[0], 
            fft_band_energies[1], 
            fft_band_energies[2], 
            fft_band_energies[3], 
            fft_band_energies[4]
        );
        printf("Brightness: [%d, %d, %d, %d, %d]\n", 
            brightness_matrix[0], 
            brightness_matrix[1], 
            brightness_matrix[2], 
            brightness_matrix[3], 
            brightness_matrix[4]
        );
        sleep_ms(10);
    }
}
