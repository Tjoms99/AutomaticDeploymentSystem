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
    uint32_t ms5847_30ba_adc_pressure = 0;
    uint32_t ms5847_30ba_adc_temperature = 0;

    // START CONVERTION
    tsys01_start_convertion();
    ms5847_30ba_start_pressure_convertion();

    // WAIT
    tsys01_wait_for_conversion();

    // GET CONVERTION VALUES
    tsys01_get_convertion(&tsys01_adc);
    ms5847_30ba_get_convertion(&ms5847_30ba_adc_pressure);

    // START CONVERTION
    ms5847_30ba_start_temperature_convertion();

    // WAIT
    tsys01_wait_for_conversion();

    // GET CONVERTION VALUES
    ms5847_30ba_get_convertion(&ms5847_30ba_adc_temperature);

    // CALCULATE VALUES
    ms5847_30ba_calculate(pressure, temperature, ms5847_30ba_adc_pressure, ms5847_30ba_adc_temperature);
    tsys01_calculate_temperature(temperature, tsys01_adc);
}

void sensors_get_depth(float *depth)
{
}
