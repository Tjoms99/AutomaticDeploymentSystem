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
float depth_register[MEMORY_SIZE];

static uint16_t read_pointer_temperature = NEXT_REGISTER_START;
static uint16_t write_pointer_temperature = NEXT_REGISTER_START;

static uint16_t read_pointer_pressure = NEXT_REGISTER_START;
static uint16_t write_pointer_pressure = NEXT_REGISTER_START;

static uint16_t read_pointer_depth = NEXT_REGISTER_START;
static uint16_t write_pointer_depth = NEXT_REGISTER_START;

// ------------------------------------------------------------------
// TEMPERATURE
void set_temperature_current_register(float temperature)
{
    temperature_register[CURRENT_REGISTER] = temperature;
}

void set_temperature_next_register(float temperature)
{

    temperature_register[(MEMORY_SIZE & write_pointer_temperature++)] = temperature;

    if ((write_pointer_temperature & MEMORY_SIZE) >= MEMORY_SIZE)
    {
        write_pointer_temperature = NEXT_REGISTER_START;
        write_pointer_temperature |= LOOP_FLAG; // Has looped flag triggered at MSB
    }
}

void get_temperature_current_register(float *temperature)
{
    *temperature = temperature_register[CURRENT_REGISTER];
}

// Will reset the pointer when reading an empty register
void get_temperature_next_register(float *temperature, uint8_t *done_reading)
{
    *done_reading = 0x00;

    // Circular register, check if write pointer has looped to read the correct order
    if ((write_pointer_temperature & LOOP_FLAG) && !(read_pointer_temperature & LOOP_FLAG))
        read_pointer_temperature = write_pointer_temperature + 1;

    // Read register
    *temperature = temperature_register[(MEMORY_SIZE & read_pointer_temperature++)];

    // Init read_pointer but keep LOOP_FLAG
    if ((read_pointer_temperature & MEMORY_SIZE) >= MEMORY_SIZE)
        read_pointer_temperature = NEXT_REGISTER_START | LOOP_FLAG;

    // Reset read_pointer when finished reading registers
    if (read_pointer_temperature == write_pointer_temperature)
    {
        read_pointer_temperature = NEXT_REGISTER_START;
        *done_reading = 0xFF;
    }
}

// ------------------------------------------------------------------
// PRESSURE
void set_pressure_current_register(float pressure)
{
    pressure_register[CURRENT_REGISTER] = pressure;
}

void set_pressure_next_register(float pressure)
{

    pressure_register[(MEMORY_SIZE & read_pointer_pressure++)] = pressure;

    if ((read_pointer_pressure & MEMORY_SIZE) >= MEMORY_SIZE)
    {
        read_pointer_pressure = NEXT_REGISTER_START;
        read_pointer_pressure |= LOOP_FLAG; // Has looped flag triggered at MSB
    }
}

void get_pressure_current_register(float *pressure)
{
    *pressure = pressure_register[CURRENT_REGISTER];
}

// Will reset the pointer when reading an empty register
void get_pressure_next_register(float *pressure, uint8_t *done_reading)
{
    *done_reading = 0x00;

    // Circular register, check if write pointer has looped to read the correct order
    if ((write_pointer_pressure & LOOP_FLAG) && !(read_pointer_pressure & LOOP_FLAG))
        read_pointer_pressure = write_pointer_pressure + 1;

    // Read register
    *pressure = pressure_register[(MEMORY_SIZE & read_pointer_pressure++)];

    // Init read_pointer but keep LOOP_FLAG
    if ((read_pointer_pressure & MEMORY_SIZE) >= MEMORY_SIZE)
        read_pointer_pressure = NEXT_REGISTER_START | LOOP_FLAG;

    // Reset read_pointer when finished reading registers
    if (read_pointer_pressure == write_pointer_pressure)
    {
        read_pointer_pressure = NEXT_REGISTER_START;
        *done_reading = 0xFF;
    }
}
