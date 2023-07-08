#ifndef __LEDS_H__
#define __LEDS_H__

void leds_init(void);

void leds_set_red(uint8_t pin_state);
void leds_set_green(uint8_t pin_state);
void leds_set_blue(uint8_t pin_state);

void leds_toggle_red(void);
void leds_toggle_green(void);
void leds_toggle_blue(void);

#endif