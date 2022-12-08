/*
 * temp.c
 *
 *  Created on: 5 Dec 2022
 *      Author: Marcus Alexander Tjomsaas
 */

#include <stdint.h>
#include "temp.h"
#include "../i2c/i2c.h"

static void set_coefficients(uint8_t coefficient)
{
    if(coefficient == K4) k4 = data_in;
    if(coefficient == K4) k3 = data_in;
    if(coefficient == K2) k2 = data_in;
    if(coefficient == K1) k1 = data_in;
    if(coefficient == K0) k0 = data_in;
}

void init_temp()
{
    uint32_t index;

    i2c_write(RESET_SENSOR, TEMPERATURE_ADDRESS);   // reset sensor
    for(index = 0; index <10000; index++);

    i2c_write(GET_K4, TEMPERATURE_ADDRESS);   // get k4
    i2c_read(READ_2_BYTES);
    set_coefficients(K4);

    i2c_write(GET_K3, TEMPERATURE_ADDRESS);   // get k3
    i2c_read(READ_2_BYTES);
    set_coefficients(K3);

    i2c_write(GET_K2, TEMPERATURE_ADDRESS);   // get k2
    i2c_read(READ_2_BYTES);
    set_coefficients(K2);

    i2c_write(GET_K1, TEMPERATURE_ADDRESS);   // get k1
    i2c_read(READ_2_BYTES);
    set_coefficients(K1);

    i2c_write(GET_K0, TEMPERATURE_ADDRESS);   // get k0
    i2c_read(READ_2_BYTES);
    set_coefficients(K0);
}


static void read_adc_temperature()
{
    uint32_t index;

    i2c_write(START_CONVERSION, TEMPERATURE_ADDRESS);   // start conversion

    for(index = 0; index <10000; index++);

    i2c_write(GET_TEMP_VALUE, TEMPERATURE_ADDRESS);   // read adc temp value
    i2c_read(READ_2_BYTES);
}
static float calculate_temperature()
{
    volatile static double adc  = 9378708/256;
    volatile static double k4_t = 4;
    volatile static double k3_t = 3;
    volatile static double k2_t = 2;
    volatile static double k1_t = 1;
    volatile static double k0_t = 0;
    volatile static double temp = 0;

    adc = data_in;

    k4_t = -1 * (2.0f * k4 / 1000000000000000000000.0f * adc * adc * adc * adc);
    k3_t =      (4.0f * k3 / 10000000000000000.0f * adc * adc * adc);
    k2_t = -1 * (2.0f * k2 / 100000000000.0f * adc * adc);
    k1_t =      (1.0f * k1 / 1000000.0f  * adc);
    k0_t = -1 * (1.5f * k0 / 100.0f);

    temp = k4_t + k3_t + k2_t + k1_t + k0_t;
    return temp;
}
double get_temperature()
{
    read_adc_temperature();
    return calculate_temperature();
}
