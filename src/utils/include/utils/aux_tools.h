#ifndef AUX_TOOLS_H
#define AUX_TOOLS_H

// AUX INPUT
#define AUX_SIGNAL_BASELINE 2075
#define MIN_DIFF 10
#define MAX_DIFF 2000

uint16_t get_aux_input_diff(uint16_t baseline_audio_val);

#endif
