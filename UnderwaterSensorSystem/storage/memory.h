/*
 * memroy.h
 *
 *  Created on: 21 May 2023
 *      Author: marcu
 */

#ifndef STORAGE_MEMORY_H_
#define STORAGE_MEMORY_H_

#include <stdint.h>
#define MEMORY_SIZE 0x1FF
#define TEMPERATURE_CURRENT_REGISTER 0x000
#define TEMPERATURE_NEXT_REGISTER_START 0x001

#define PRESSURE_CURRENT_REGISTER 0x000
#define PRESSURE_NEXT_REGISTER_START 0x001



#define LOOP_FLAG 0x8000

void set_temperature_current_register(float temperature);

void set_temperature_next_register(float temperature);

void get_temperature_current_register(float *temperature);

void get_temperature_next_register(float *temperature, uint8_t *done_reading);

#endif /* STORAGE_MEMORY_H_ */
