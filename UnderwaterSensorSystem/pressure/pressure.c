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
static float temp = 0;

void init_pressure()
{
    i2c_write(RESET_SENSOR, PRESSURE_ADDRESS);
    for(i = 0; i <10000; i++);

    for(i = 0; i < 7; i++)
    {
        i2c_write((PROM_READ + i*2), PRESSURE_ADDRESS);
        i2c_read(BYTES_2, PRESSURE_ADDRESS);
        constants[i] = data_in;
    }
    // Verify that data is correct with CRC
    uint8_t crcRead = constants[0] >> 12;
    uint8_t crcCalculated = crc4(constants);

    if ( crcCalculated != crcRead ) {
        i = 69;
    }
}

void measure()
{
    i2c_write(START_CONVERTION_D1_4096, PRESSURE_ADDRESS);
    for(i = 0; i <10000; i++);
    i2c_write(GET_ADC_VALUE, PRESSURE_ADDRESS);
    i2c_read(BYTES_3, PRESSURE_ADDRESS);
    D1_pres = data_in & 0xFFFFFF;

    i2c_write(START_CONVERTION_D2_4096, PRESSURE_ADDRESS);
    for(i = 0; i <10000; i++);
    i2c_write(GET_ADC_VALUE, PRESSURE_ADDRESS);
    i2c_read(BYTES_3, PRESSURE_ADDRESS);
    D2_temp = data_in & 0xFFFFFF;

    calculate();

}
static void calculate()
{
    // Given C1-C6 and D1, D2, calculated TEMP and P
    // Do conversion first and then second order temp compensation

    static volatile int32_t dT = 0;
    static volatile int64_t SENS = 0;
    static volatile int64_t OFF = 0;
    static volatile int32_t SENSi = 0;
    static volatile int32_t OFFi = 0;
    static volatile int32_t Ti = 0;
    static volatile int64_t OFF2 = 0;
    static volatile int64_t SENS2 = 0;

    constants[1] = 34982;
    constants[2] = 36352;
    constants[3] = 20328;
    constants[4] = 22354;
    constants[5] = 26646;
    constants[6] = 26146;

    D1_pres = 4958179;
    D2_temp = 6815414;


    // Temperature conversion 1st order
   const uint32_t tref = (uint32_t)constants[5] * 256l;
   dT = D2_temp - tref;

   const int64_t tempsens = constants[6] * (int64_t)dT / 8388608LL;
   TEMP = 2000l + tempsens;

   // Pressure conversion 1st order
   SENS = (int64_t)constants[1] * 32768l + (int64_t)constants[3] * dT / 256l;
   OFF = (int64_t)constants[2] * 65536l + (int64_t)constants[4] * dT / 128l;
   P = (D1_pres * SENS / 2097152l - OFF) / (8192l);

   // Temperature conversion 2st order
   if((TEMP  / 100) < 20){         //Low temp
               Ti = (3 * (int64_t)dT * (int64_t)dT) / 8589934592L;
               OFFi = (3 * (TEMP - 2000) * (TEMP - 2000)) / 2;
               SENSi = (5 * (TEMP - 2000) * (TEMP - 2000)) / 8;
               if((TEMP / 100) < -15){    //Very low temp
                   OFFi = OFFi + 7* (TEMP + 1500l) * (TEMP + 1500l);
                   SENSi = SENSi + 4* (TEMP + 1500l) * (TEMP + 1500l);
               }
           }
   else if((TEMP / 100) >= 20){    //High temp
          Ti = 2 * ((int64_t)dT * (int64_t)dT) / (137438953472LL);
          OFFi = (1 * (TEMP - 2000) * (TEMP - 2000)) / 16;
          SENSi = 0;
      }

      TEMP = TEMP - Ti;

      // Pressure conversion 2st order
     OFF2 = OFF - OFFi;
     SENS2 = SENS - SENSi;

     P = (D1_pres * SENS2 / 2097152l - OFF2) / 8192l;
}

uint16_t get_pressure()
{
    return P/10; //mBar
}

uint16_t get_temperature_p()
{
   return TEMP;
}

// The pressure sensor measures absolute pressure, so it will measure the atmospheric pressure + water pressure
// We subtract the atmospheric pressure to calculate the depth with only the water pressure
// The average atmospheric pressure of 101300 pascal is used for the calcuation, but atmospheric pressure varies
// If the atmospheric pressure is not 101300 at the time of reading, the depth reported will be offset
// In order to calculate the correct depth, the actual atmospheric pressure should be measured once in air, and
// that value should subtracted for subsequent depth calculations.
float get_depth() {
    return (get_pressure(Pa)-101300)/(fluidDensity*9.80665);
}

float altitude() {
    return 1;//(1-pow((pressure()/1013.25),.190284))*145366.45*.3048;
}

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
