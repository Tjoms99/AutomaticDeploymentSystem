/*
 * memroy.h
 *
 *  Created on: 21 May 2023
 *      Author: marcu
 */

#ifndef STORAGE_MEMORY_H_
#define STORAGE_MEMORY_H_

#include <stdint.h>
#define MEMORY_SIZE 0x0FF
#define CURRENT_REGISTER 0x000
#define NEXT_REGISTER_START 0x001

#define LOOP_FLAG 0x8000

void set_temperature_current_register(float temperature);

void set_temperature_next_register(float temperature);

void get_temperature_current_register(float *temperature);

void get_temperature_next_register(float *temperature, uint8_t *done_reading);

void set_pressure_current_register(float pressure);

void set_pressure_next_register(float pressure);

void get_pressure_current_register(float *pressure);

void set_depth_current_register(float depth);

void set_depth_next_register(float depth);

void get_depth_current_register(float *depth);

#endif /* STORAGE_MEMORY_H_ */
