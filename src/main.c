#include <stdio.h>
#include "pico/stdlib.h"
#include "prototypes/ws2812_multi.h"

int main() {
    stdio_init_all();
    sleep_ms(2000);

    ws2812_multi();

    return 0;
}
