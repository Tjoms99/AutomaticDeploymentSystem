
#ifndef __APPLICATION_CLOCK_H__
#define __APPLICATION_CLOCK_H__
#include <stdint.h>

#define SYSTEM_ON BIT7
#define PRINT_ALL_REGISTERS BIT6
#define PRINT_CURRENT_REGISTER BIT5
#define VOLT12_ENABLE BIT4
#define RS232_ENABLE BIT3
#define DEPTH_ZERO BIT2
#define CUSTOM_TIME BIT1
#define SENSOR_SAMPLE BIT0


void state_machine_get_flag(uint8_t *flag);

void state_machine_set_flag(uint8_t flag);

void state_machine_remove_flag(uint8_t flag);

void state_machine_update_flag(char data);

void state_machine_set_loop_interval();

void state_machine_reset(void);

void state_machine_loop(void);

void state_machine_init(void);

#endif
