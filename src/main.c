#include <stdio.h>
#include "pico/stdlib.h"
#include "utils/fft_5_band.h"
#include "utils/fft_5_band.h"

int main() {
    stdio_init_all();
    
    five_band_fft_loop();

    return 0;
}
