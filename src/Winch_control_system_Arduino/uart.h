#ifndef _UART_H_
#define _UART_H_

void uart_begin(void);
void uart_write_char(char *buffer);
void uart_read(char **buffer);
void uart_write_reset(void);
void uart_write_sample(void);
void uart_write_init_depth(void);
void uart_write_sample_time(char *sampling_interval);
void uart_write_rs232(void);
void uart_write_12v(void);
#endif