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
    if(coefficient == 4) k4 = data_in;
    if(coefficient == 3) k3 = data_in;
    if(coefficient == 2) k2 = data_in;
    if(coefficient == 1) k1 = data_in;
    if(coefficient == 0) k0 = data_in;
}

void init_temp()
{
    uint32_t index;

    i2c_write(0x1E);   // reset sensor
    for(index = 0; index <10000; index++);

    i2c_write(0xA2);   // get k4
    i2c_read(2);
    set_coefficients(4);

    i2c_write(0xA4);   // get k3
    i2c_read(2);
    set_coefficients(3);

    i2c_write(0xA6);   // get k2
    i2c_read(2);
    set_coefficients(2);

    i2c_write(0xA8);   // get k1
    i2c_read(2);
    set_coefficients(1);

    i2c_write(0xAA);   // get k0
    i2c_read(2);
    set_coefficients(0);
}


static void read_adc_temperature()
{
    uint32_t index;

    i2c_write(0x48);   // start conversion
    for(index = 0; index <10000; index++);
    i2c_write(0x00);   // read adc temp value
    i2c_read(2);
}
static double calculate_temperature()
{
    volatile static double adc  = 9378708;
    volatile static double temp = 10;

    adc = data_in;

    volatile float k4_t = -1 * (2.0f * k4 / 1000000000000000000000.0f * adc * adc * adc * adc);
    volatile float k3_t =      (4.0f * k3 / 10000000000000000.0f * adc * adc * adc);
    volatile float k2_t = -1 * (2.0f * k2 / 100000000000.0f * adc * adc);
    volatile float k1_t =      (1.0f * k1 / 1000000.0f  * adc);
    volatile float k0_t = -1 * (1.5f * k0 / 100.0f);

    temp = k4_t + k3_t + k2_t + k1_t + k0_t;
    return temp;
}
double get_temperature()
{
    read_adc_temperature();
    return calculate_temperature();
}
