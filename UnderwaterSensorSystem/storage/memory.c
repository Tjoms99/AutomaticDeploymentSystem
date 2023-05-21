/*
 * memory.c
 *
 *  Created on: 21 May 2023
 *      Author: marcu
 */

#include "../power/power.h"
#include "memory.h"

static float memory[0x1FFF];
static uint16_t pressure_pointer = 0;

#define CONTINUOUS_MODE 0x04
#define SINGLE_MODE 0x03
#define CUSTOM_TIME 0x02
#define RS232_ENABLE 0x01
#define POWER_ENABLE 0x00

static void update_status(){
   if((int) memory[CONTROL_REGISTER] & CONTINUOUS_MODE){

   }

   if((int) memory[CONTROL_REGISTER] & SINGLE_MODE){

      }

   if((int)memory[CONTROL_REGISTER] & CUSTOM_TIME){

      }

   if((int)memory[CONTROL_REGISTER] & RS232_ENABLE){

      }
   if((int)memory[CONTROL_REGISTER] & POWER_ENABLE){

      }
}

void set_control_register(float data){
    memory[CONTROL_REGISTER] = data;

    update_status();

}

void set_time_register(float data){
    memory[TIME_REGISTER] = data;
}

void get_temperature_current_register(float data){
    memory[TEMPERATURE_CURRENT_REGISTER] = data;
}

void get_temperature_next_register(float data){
    memory[TEMPERATURE_CURRENT_REGISTER] = data;
}

void set_temperature_next_register(float data){
    static uint16_t temperature_pointer = 0;

    if(TEMPERATURE_NEXT_REGISTER_END < TEMPERATURE_NEXT_REGISTER_START + temperature_pointer){
        temperature_pointer = 0;
    }


    memory[TEMPERATURE_NEXT_REGISTER_START + temperature_pointer] = data;
    temperature_pointer++;

}

void get_pressure_current_register(float data){
    memory[PRESSURE_CURRENT_REGISTER] = data;
}

void set_pressure_next_register(float data){
    memory[PRESSURE_NEXT_REGISTER_START + pressure_pointer] = data;
}
void get_pressure_next_register(float data){
    memory[PRESSURE_CURRENT_REGISTER] = data;
}
