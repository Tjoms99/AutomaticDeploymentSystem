#ifndef _UART_H_
#define _UART_H_

void uart_begin(void);
void uart_write(char *buffer);
void uart_read(char **buffer);
#endif