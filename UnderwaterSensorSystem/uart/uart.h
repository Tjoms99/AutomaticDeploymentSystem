/*
 * uart.h
 *
 *  Created on: 31 Jan 2023
 *      Author: marcu
 */

#ifndef UART_UART_H_
#define UART_UART_H_
#include <stdint.h>

    void uart_init(void);
    void uart_set_mode(uint8_t mode);
    void uart_transmit(char data);
    void uart_recieve(char *data);



#endif /* UART_UART_H_ */
