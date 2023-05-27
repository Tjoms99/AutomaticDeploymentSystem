/*
 * uart.h
 *
 *  Created on: 31 Jan 2023
 *      Author: marcu
 */

#ifndef RS485_MAX3471_H_
#define RS485_MAX3471_H_
#include <stdint.h>

void max3471_init(void);
void max3471_set_mode(uint8_t mode);
void max3471_transmit(char data);
void max3471_recieve(char *data);

#endif /* RS485_MAX3471_H_ */
