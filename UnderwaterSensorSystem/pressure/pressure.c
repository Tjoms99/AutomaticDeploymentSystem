/*
 * pressure.constants
 *
 *  Created on: 7 Dec 2022
 *      Author: marcu
 */

#include <stdint.h>
#include "../i2c/i2c.h"
#include "pressure.h"
uint32_t i = 0;

void init_pressure()
{
    i2c_write(TSYS01_RESET_SENSOR, PRESSURE_ADDRESS);   // reset sensor
    for(i = 0; i <10000; i++);

    for(i = 0; i < 7; i++)
    {
        i2c_write((PROM_READ + i*2), PRESSURE_ADDRESS); // get coefficients
        i2c_read(BYTES_2, PRESSURE_ADDRESS);
        coefficients[i] = data_in;
    }

    uint8_t crcRead = coefficients[0] >> 12;
    uint8_t crcCalculated = crc4(coefficients);
    if ( crcCalculated != crcRead )  i = 69;            // Verify that data is correct with CRC
}

void measure()
{
    i2c_write(START_CONVERTION_D1_4096, PRESSURE_ADDRESS);  // start conversion of pressure
    for(i = 0; i <10000; i++);
    i2c_write(GET_ADC_VALUE_P, PRESSURE_ADDRESS);
    i2c_read(BYTES_3, PRESSURE_ADDRESS);                    // set ADC value (pressure)
    D1_pres = data_in;

    i2c_write(START_CONVERTION_D2_4096, PRESSURE_ADDRESS);  // start conversion of temperature
    for(i = 0; i <10000; i++);
    i2c_write(GET_ADC_VALUE_P, PRESSURE_ADDRESS);
    i2c_read(BYTES_3, PRESSURE_ADDRESS);
    D2_temp = data_in;                                      // set ADC value (temperature)

    calculate();                                            // calculate pressure and temperature

    if(start_Pa == 0) start_Pa = get_pressure(Pa);          // set initial pressure (used to measure depth)
}

// set values given in the datasheet (for testing)
static void init_test_values()
{
    coefficients[1] = 34982;
    coefficients[2] = 36352;
    coefficients[3] = 20328;
    coefficients[4] = 22354;
    coefficients[5] = 26646;
    coefficients[6] = 26146;

    D1_pres = 4958179;
    D2_temp = 6815414;
}

static void first_order_calculations()
{

}
static void calculate()
{
    static volatile int32_t dT = 0;
    static volatile int64_t SENS = 0;
    static volatile int64_t OFF = 0;
    static volatile int32_t SENSi = 0;
    static volatile int32_t OFFi = 0;
    static volatile int32_t Ti = 0;
    static volatile int64_t OFF2 = 0;
    static volatile int64_t SENS2 = 0;


    //init_test_values();

    // Temperature conversion 1st order
   dT = D2_temp - (uint32_t)coefficients[5] * 256l;
   TEMP = 2000l + coefficients[6] * (int64_t)dT / 8388608LL;

   // Pressure conversion 1st order
   SENS = (int64_t)coefficients[1] * 32768l + (int64_t)coefficients[3] * dT / 256l;
   OFF = (int64_t)coefficients[2] * 65536l + (int64_t)coefficients[4] * dT / 128l;
   P = (D1_pres * SENS / 2097152l - OFF) / (8192l);


   // Second order pressure and temperature calculations
   if((TEMP / 100) >= 20)   //High temp
   {
       Ti = 2 * ((int64_t)dT * (int64_t)dT) / (137438953472LL);
       OFFi = (1 * (TEMP - 2000) * (TEMP - 2000)) / 16;
       SENSi = 0;
   }

   if((TEMP  / 100) < 20)   //Low temp
   {
       Ti = (3 * (int64_t)dT * (int64_t)dT) / 8589934592L;
       OFFi = (3 * (TEMP - 2000) * (TEMP - 2000)) / 2;
       SENSi = (5 * (TEMP - 2000) * (TEMP - 2000)) / 8;
   }

   if((TEMP / 100) < -15)   //Very low temp
   {
       OFFi = OFFi + 7* (TEMP + 1500l) * (TEMP + 1500l);
       SENSi = SENSi + 4* (TEMP + 1500l) * (TEMP + 1500l);
   }

   OFF2 = OFF - OFFi;
   SENS2 = SENS - SENSi;

   // Pressure & temperature
   P = (D1_pres * SENS2 / 2097152l - OFF2) / 8192l;
   TEMP = TEMP - Ti;
}

float get_pressure(float conversion)
{
    return P*conversion/10; //mBar
}

float get_temperature_p()
{
   return TEMP/ 100.0f;
}



// The pressure sensor measures absolute pressure, so it will measure the atmospheric pressure + water pressure
// We subtract the atmospheric pressure to calculate the depth with only the water pressure
// The average atmospheric pressure of 101300 pascal is used for the calcuation, but atmospheric pressure varies
// If the atmospheric pressure is not 101300 at the time of reading, the depth reported will be offset
// In order to calculate the correct depth, the actual atmospheric pressure should be measured once in air, and
// that value should subtracted for subsequent depth calculations.
float get_depth() {
    return (get_pressure(Pa)-start_Pa)/(FRESHWATER_DENSITY*9.80665); // h = P/(R*g)
}

//Cyclic redundancy check
static uint8_t crc4(uint16_t n_prom[]) {
    uint16_t n_rem = 0;
    uint8_t n_bit = 0;

    n_prom[0] = ((n_prom[0]) & 0x0FFF);
    n_prom[7] = 0;

    for ( i = 0 ; i < 16; i++ ) {
        if ( i%2 == 1 ) {
            n_rem ^= (uint16_t)((n_prom[i>>1]) & 0x00FF);
        } else {
            n_rem ^= (uint16_t)(n_prom[i>>1] >> 8);
        }
        for ( n_bit = 8 ; n_bit > 0 ; n_bit-- ) {
            if ( n_rem & 0x8000 ) {
                n_rem = (n_rem << 1) ^ 0x3000;
            } else {
                n_rem = (n_rem << 1);
            }
        }
    }

    n_rem = ((n_rem >> 12) & 0x000F);

    return n_rem ^ 0x00;
}
