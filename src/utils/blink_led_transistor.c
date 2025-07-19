#include "pico/stdlib.h"
#include "utils/blink_led_transistor.h"

#define TRANS_BASE_PIN 20
#define ONBOARD_LED 25

void blink_led_transistor() {
    gpio_init(TRANS_BASE_PIN);
    gpio_set_dir(TRANS_BASE_PIN, GPIO_OUT);

    gpio_init(ONBOARD_LED);
    gpio_set_dir(ONBOARD_LED, GPIO_OUT);

    while(true) {
        gpio_put(ONBOARD_LED, 1);
        gpio_put(TRANS_BASE_PIN, 1);
        sleep_ms(1000);

        gpio_put(ONBOARD_LED, 0);
        gpio_put(TRANS_BASE_PIN, 0);
        sleep_ms(1000);
    }
}
