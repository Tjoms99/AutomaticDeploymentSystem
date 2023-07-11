
#ifndef __APPLICATION_CLOCK_H__
#define __APPLICATION_CLOCK_H__
#include <stdint.h>

#define SYSTEM_ON BIT7
#define PRINT_ALL_REGISTERS BIT6
#define PRINT_CURRENT_REGISTER BIT5
#define CONTINUOUS_MODE BIT4
#define DEPTH_ZERO BIT3
#define CUSTOM_TIME BIT2
#define RS232_ENABLE BIT1
#define VOLT12_ENABLE BIT0

void state_machine_get_flag(uint8_t *flag);

void state_machine_set_flag(uint8_t flag);

void state_machine_remove_flag(uint8_t flag);

void state_machine_update_flag(char data);

void state_machine_set_loop_interval(char seconds_c);

void state_machine_reset(void);

void state_machine_loop(void);

void state_machine_init(void);

#endif
