/*
 * memroy.h
 *
 *  Created on: 21 May 2023
 *      Author: marcu
 */

#ifndef STORAGE_MEMORY_H_
#define STORAGE_MEMORY_H_

#define CONTROL_REGISTER 0x0000
#define TIME_REGISTER 0x0001
#define TEMPERATURE_CURRENT_REGISTER 0x0002
#define TEMPERATURE_NEXT_REGISTER_START 0x0004
#define TEMPERATURE_NEXT_REGISTER_END 0x0FFF
#define PRESSURE_CURRENT_REGISTER 0x0003
#define PRESSURE_NEXT_REGISTER_START 0x1000
#define PRESSURE_NEXT_REGISTER_END 0x1FFF


/**
 * Control register
 * 4 - Single mode: One pressure + temperature sample
 * 3 - Continuous mode: Continuously sample pressure and temperature
 * 2 - Enable custom time interval used in continuous mode
 * 1 - RS232 enable
 * 0 - Power enable: 12V
 */
void set_control_register(float data);

void set_time_register(float data);

void get_temperature_current_register(float data);

void set_temperature_next_register(float data);

void get_temperature_next_register(float data);

void get_pressure_current_register(float data);

void set_pressure_next_register(float data);

void get_pressure_next_register(float data);

#endif /* STORAGE_MEMORY_H_ */
