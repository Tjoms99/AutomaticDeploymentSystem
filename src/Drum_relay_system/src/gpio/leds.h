#ifndef __LEDS_H__
#define __LEDS_H__
#include <stdint.h>

/**
 * @brief Sets red LED.
 *
 * @param[in]  pin_state If 0 then LED off, else LED on.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int leds_set_red(uint8_t pin_state);

/**
 * @brief Sets green LED.
 *
 * @param[in]  pin_state If 0 then LED off, else LED on.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int leds_set_green(uint8_t pin_state);

/**
 * @brief Sets blue LED.
 *
 * @param[in] pin_state If 0 then LED off, else LED on.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int leds_set_blue(uint8_t pin_state);

/**
 * @brief Toggles red LED.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int leds_toggle_red(void);

/**
 * @brief Toggles green LED.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int leds_toggle_green(void);

/**
 * @brief Toggles blue LED.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int leds_toggle_blue(void);

/**
 * @brief Initialize the RGB user leds on the XIAO BLE sense.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int leds_init(void);

#endif