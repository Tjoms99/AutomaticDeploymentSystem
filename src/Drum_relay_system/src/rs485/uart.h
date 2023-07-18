#ifndef __UART_H__
#define __UART_H__
#include <stdio.h>

/**
 * @brief Checks if the UART is busy.
 * UART is busy when the Underwater Sensor System has transmitted an 'b' character,
 * and is not busy when the USS has transmitted an 'r' (ready) character.
 *
 * @retval True when busy, false when not busy.
 */
bool uart_get_is_busy(void);

/**
 * @brief Write to the UART poll register.
 */
void uart_write(char *message);

/**
 * @brief Initialize UART with speed at 56000.
 *
 * @retval Negative errno number on error.
 */
int uart_init(void);

#endif