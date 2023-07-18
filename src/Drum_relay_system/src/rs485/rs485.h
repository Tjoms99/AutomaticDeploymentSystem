#ifndef __RS485_H__
#define __RS485_H__

/**
 * @brief Writes a message to the UART driver.
 * Waits for the UART to become ready if the UART line is busy.
 *
 * @note Cannot recieve messages when transmitting due to the half duplex protocol.
 */
void rs485_write(char *message);

/**
 * @brief Initialize RS485.
 *
 * @retval 0 if successful. Negative errno number on error.
 */
int rs485_init(void);

#endif