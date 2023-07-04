#ifndef _UART_H_
#define _UART_H_

typedef void (*uart_callback_uss_ready)();
typedef void (*uart_callback_data_ready)(char temperature[6], char pressure[6], char depth[6]);
void uart_begin(void);
void uart_write_char(char *buffer);
void uart_read(char **buffer);
void uart_write_reset(void);
void uart_write_sample(void);
void uart_write_init_depth(void);
void uart_write_sample_time(char *sampling_interval);
void uart_write_rs232(void);
void uart_write_12v(void);
void uart_register_callback_data_ready(uart_callback_data_ready cb);
#endif