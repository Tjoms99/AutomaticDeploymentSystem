
#include <stdint.h>
#include <temperature/TSYS01.h>
#include <library/conversions.h>

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
    char buffer[6];

    get_pressure_current_register(&pressure);
    float_to_char_array(pressure, buffer);

    max3471_transmit_6_bytes(buffer);
}

void print_current_values()
{
    print_current_temperature();
    // print_current_pressure();
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
