#ifndef __RS485_H__
#define __RS485_H__

void rs485_tx_enable(void);

void rs485_tx_disable(void);

void rs485_write(char *message);

void rs485_init(void);

#endif