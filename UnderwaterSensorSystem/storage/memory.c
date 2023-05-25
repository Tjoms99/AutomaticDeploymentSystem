/*
 * memory.c
 *
 *  Created on: 21 May 2023
 *      Author: marcu
 */

#include "memory.h"

uint16_t custom_timer_register;
float temperature_register[MEMORY_SIZE];
float pressure_register[MEMORY_SIZE];

static uint16_t read_pointer = TEMPERATURE_NEXT_REGISTER_START;
static uint16_t write_pointer = TEMPERATURE_NEXT_REGISTER_START;

void set_temperature_current_register(float temperature)
{
    temperature_register[TEMPERATURE_CURRENT_REGISTER] = temperature;
}

void set_temperature_next_register(float temperature)
{

    temperature_register[(MEMORY_SIZE & write_pointer++)] = temperature;

    if ((write_pointer & MEMORY_SIZE) >= MEMORY_SIZE)
    {
        write_pointer = TEMPERATURE_NEXT_REGISTER_START;
        write_pointer |= LOOP_FLAG; // Has looped flag triggered at MSB
    }
}

void get_temperature_current_register(float *temperature)
{
    *temperature = temperature_register[TEMPERATURE_CURRENT_REGISTER];
}

// Will reset the pointer when reading an empty register
void get_temperature_next_register(float *temperature, uint8_t *done_reading)
{
    *done_reading = 0x00;

    // Circular register, check if write pointer has looped to read the correct order
    if ((write_pointer & LOOP_FLAG) && !(read_pointer & LOOP_FLAG))
        read_pointer = write_pointer + 1;

    // Read register
    *temperature = temperature_register[(MEMORY_SIZE & read_pointer++)];

    // Init read_pointer but keep LOOP_FLAG
    if ((read_pointer & MEMORY_SIZE) >= MEMORY_SIZE)
        read_pointer = TEMPERATURE_NEXT_REGISTER_START | LOOP_FLAG;

    // Reset read_pointer when finished reading registers
    if (read_pointer == write_pointer)
    {
        read_pointer = TEMPERATURE_NEXT_REGISTER_START;
        *done_reading = 0xFF;
    }
}
