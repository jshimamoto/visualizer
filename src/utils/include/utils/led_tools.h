#ifndef LED_TOOLS_H
#define LED_TOOLS_H

void light_onboard_led(uint8_t GPIO_PIN_NUMBER);

uint16_t get_baseline_mic_input(uint16_t NUM_SAMPLES);

uint16_t read_mic();

#endif
