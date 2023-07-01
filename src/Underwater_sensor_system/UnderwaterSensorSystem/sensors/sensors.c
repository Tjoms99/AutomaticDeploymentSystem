/*
 * sensors.c
 *
 *  Created on: 1 Jul 2023
 *      Author: marcu
 */

#include <sensors/pressure/MS5837_30BA.h>
#include <sensors/temperature/TSYS01.h>

void sensors_init()
{
    tsys01_init();
    ms5847_30ba_init();
}

void sensors_get_values(float *temperature, float *pressure)
{
    float tsys01_adc = 0;

    tsys01_start_convertion();
    tsys01_wait_for_conversion();
    tsys01_get_convertion(&tsys01_adc);
    tsys01_calculate_temperature(temperature, &tsys01_adc);
}

void sensors_get_depth(float *depth)
{
}
