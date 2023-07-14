#ifndef _UART_H_
#define _UART_H_

typedef void (*uart_callback_data_ready)();
void uart_begin(void);
void uart_write_char(char *buffer);
void uart_register_callback_data_ready(uart_callback_data_ready cb);
#endif