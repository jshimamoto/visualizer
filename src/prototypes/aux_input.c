#include "prototypes/aux_input.h"
#include "utils/led_tools.h"

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

void main() {
    // Initialize inputs and outputs
    stdio_init_all();
    sleep_ms(2000);
    adc_init();
    adc_gpio_init(AUX_PIN);    
    adc_select_input(ADC_INPUT);

    while (true) {;
        uint16_t max = 4095;
        uint16_t min = 0;

        uint16_t val = adc_read();
        if (val < min) val = min;
        if (val > max) val = max;

        printf("Signal: %u\n", val);
        sleep_ms(100);
    }
}
