#ifndef SAMPLING_TOOLS_H
#define SAMPLING_TOOLS_H

// ADC
#define ADC_INPUT 2
#define ADC_PIN 28

#define INPUT_MODE "AUX"

extern volatile uint16_t *write_buffer;
extern volatile uint16_t *read_buffer;

extern volatile bool new_data_ready;

void sampling_core();

#endif
