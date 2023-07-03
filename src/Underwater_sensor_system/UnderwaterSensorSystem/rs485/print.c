
#include <stdint.h>
#include <sensors/temperature/TSYS01.h>
#include <library/conversions.h>
#include <storage/memory.h>
#include <rs485/max3471.h>
#include "print.h"

static const char temperature_tag = 't';
static const char pressure_tag = 'p';
static const char depth_tag = 'd';

void print_current_temperature()
{
    float temperature = 0.0;
    char buffer[7];

    get_temperature_current_register(&temperature);
    float_to_char_array(temperature, buffer);
    buffer[6] = '\0';

    max3471_transmit_6_bytes(buffer, temperature_tag);
}

void print_current_pressure()
{
    float pressure = 0.0;
    int32_t pressure_Pa;
    char buffer[7];

    get_pressure_current_register(&pressure);
    pressure_Pa = (int)pressure;
    pressure_Pa *= 10;
    itoa(pressure_Pa, buffer, 10);
    buffer[6] = '\0';

    max3471_transmit_6_bytes(buffer, pressure_tag);
}

void print_current_depth()
{
    float depth = 0.0;
    char buffer[7];

    get_depth_current_register(&depth);
    float_to_char_array(depth, buffer);
    buffer[6] = '\0';

    max3471_transmit_6_bytes(buffer, depth_tag);
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
        max3471_transmit_6_bytes(buffer, temperature_tag);
    } while (!done_reading);
}
