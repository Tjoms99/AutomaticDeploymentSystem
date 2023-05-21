/*
 * memroy.h
 *
 *  Created on: 21 May 2023
 *      Author: marcu
 */

#ifndef STORAGE_MEMORY_H_
#define STORAGE_MEMORY_H_

#include <stdint.h>


#define TEMPERATURE_CURRENT_REGISTER 0x000
#define TEMPERATURE_NEXT_REGISTER_START 0x001
#define TEMPERATURE_NEXT_REGISTER_END 0x1FF

#define PRESSURE_CURRENT_REGISTER 0x000
#define PRESSURE_NEXT_REGISTER_START 0x001
#define PRESSURE_NEXT_REGISTER_END 0x1FF

#define CONTINUOUS_MODE 0x10
#define SINGLE_MODE 0x08
#define CUSTOM_TIME 0x04
#define RS232_ENABLE 0x02
#define POWER_ENABLE 0x01

/**
 * Control register
 * 4 - Single mode: One pressure + temperature sample
 * 3 - Continuous mode: Continuously sample pressure and temperature
 * 2 - Enable custom time interval used in continuous mode
 * 1 - RS232 enable
 * 0 - Power enable: 12V
 */
void set_control_register(uint8_t data);

void set_temperature_current_register(float data);


void get_control_register(uint8_t *data);

void get_temperature_current_register(float *temperature);

#endif /* STORAGE_MEMORY_H_ */
