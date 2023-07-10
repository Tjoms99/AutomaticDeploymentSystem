#ifndef __UART_H__
#define __UART_H__
#include <stdio.h>

typedef void (*uart_callback_uss_ready)(char *data);

void uart_init(void);

bool uart_get_is_busy(void);

void uart_write(char *message);

#endif