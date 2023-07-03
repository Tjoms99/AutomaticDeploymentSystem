
#include <stdint.h>
#include <sensors/temperature/TSYS01.h>
#include <library/conversions.h>
#include <storage/memory.h>
#include <rs485/max3471.h>
#include "print.h"

void print_current_temperature()
{
    float temperature = 0.0;
    char buffer[6];

    get_temperature_current_register(&temperature);
    float_to_char_array(temperature, buffer);

    max3471_transmit_6_bytes(buffer);
}

void print_current_pressure()
{
    float pressure = 0.0;
    int32_t pressure_Pa;
    char buffer[6];

    get_pressure_current_register(&pressure);
    pressure_Pa = (int)pressure;
    pressure_Pa *= 10;
    itoa(pressure_Pa, buffer, 10);

    max3471_transmit_6_bytes(buffer);
}

void print_current_depth()
{
    float depth = 0.0;
    char buffer[6];

    get_depth_current_register(&depth);
    float_to_char_array(depth, buffer);

    max3471_transmit_6_bytes(buffer);
}

void print_current_values()
{
    print_current_temperature();
    print_current_pressure();
    print_current_depth();
}

void print_temperature_register()
{
    float temperature = 0.0;
    char buffer[6];
    uint8_t done_reading = 0x00;

    do
    {
        get_temperature_next_register(&temperature, &done_reading);

        float_to_char_array(temperature, buffer);
        max3471_transmit_6_bytes(buffer);
    } while (!done_reading);
}
