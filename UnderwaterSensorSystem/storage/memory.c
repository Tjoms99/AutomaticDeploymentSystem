/*
 * memory.c
 *
 *  Created on: 21 May 2023
 *      Author: marcu
 */

#include "memory.h"


uint8_t memory_control_register;
uint16_t custom_timer_register;
float temperature_register[0x1FF];
float pressure_register[0x1FF];


void set_control_register(uint8_t data){
    memory_control_register= data;
}

void set_temperature_current_register(float data){
    temperature_register[TEMPERATURE_CURRENT_REGISTER] = data;
}



void get_control_register(uint8_t *data){
    *data = memory_control_register;
}

void get_temperature_current_register(float *temperature){
   *temperature =  temperature_register[TEMPERATURE_CURRENT_REGISTER];
}
