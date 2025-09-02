#include <stdio.h>
#include "pico/stdlib.h"
#include "utils/fft_5_band.h"
#include "utils/print_amplitude.h"

int main() {
    stdio_init_all();
    sleep_ms(2000);

    five_band_fft_loop();

    return 0;
}
