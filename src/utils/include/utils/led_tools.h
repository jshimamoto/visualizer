#ifndef LED_TOOLS_H
#define LED_TOOLS_H

void light_onboard_led(uint8_t GPIO_PIN_NUMBER);

void init_transistor_pin_out(uint GPIO_PIN_NUMBER);

void setup_pwm(uint GPIO_PIN);

#endif
