/*
 * sensors.c
 *
 *  Created on: 1 Jul 2023
 *      Author: marcu
 */

#include <sensors/pressure/MS5837_30BA.h>
#include <sensors/temperature/TSYS01.h>
#include <storage/memory.h>
#include <rs485/print.h>

static float pressure_at_depth_zero = 0;

static void sensors_get_values(float *temperature, float *pressure, float *depth)
{
    float ms5847_30ba_depth = 0;
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
    ms5847_30ba_get_depth(depth, *pressure, pressure_at_depth_zero);
    tsys01_calculate_temperature(temperature, tsys01_adc);
}

static void sensors_set_pressure_at_depth_zero(float pressure)
{
    pressure_at_depth_zero = pressure;
}

// Sample pressure and temperature once
void sensors_sample()
{
    static float tsys01_temperature = 0;
    static float ms5847_30ba_pressure = 0;
    static float ms5847_30ba_depth = 0;

    sensors_get_values(&tsys01_temperature, &ms5847_30ba_pressure, &ms5847_30ba_depth);

    set_temperature_current_register(tsys01_temperature);
    set_temperature_next_register(tsys01_temperature);

    set_pressure_current_register(ms5847_30ba_pressure);
    set_pressure_next_register(ms5847_30ba_pressure);

    set_depth_current_register(ms5847_30ba_depth);
    set_depth_next_register(ms5847_30ba_depth);
}

void sensors_sample_and_print()
{
    sensors_sample();
    print_current_values();
}

void sensors_set_depth_zero()
{
    static float ms5847_30ba_pressure = 0;
    get_pressure_current_register(&ms5847_30ba_pressure);
    sensors_set_pressure_at_depth_zero(ms5847_30ba_pressure);
}

void sensors_init()
{
    tsys01_init();
    ms5847_30ba_init();
}