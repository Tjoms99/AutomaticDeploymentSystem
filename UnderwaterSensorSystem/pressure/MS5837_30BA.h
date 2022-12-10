/*
 * pressure.h
 *
 *  Created on: 7 Dec 2022
 *      Author: marcu
 */

#ifndef MS5837_30BA_H_
#define MS5837_30BA_H_
#include <stdint.h>

    #define MS5837_30BA_ADDRESS 0x76

    #define MS5837_30BA_RESET_SENSOR                0x1E
    #define MS5837_30BA_GET_ADC_VALUE               0x00
    #define MS5837_30BA_PROM_READ                   0xA0
    #define MS5837_30BA_START_CONVERTION_D1_4096    0x48
    #define MS5837_30BA_START_CONVERTION_D2_4096    0x58

    #define BYTES_2 0X02
    #define BYTES_3 0X03

    #define SEAWATER_DENSITY    1029  //kg/m^3
    #define FRESHWATER_DENSITY  1000 //kg/m^3

    static uint16_t coefficients[8];

    void  MS5837_30BA_init(void);
    void  MS5837_30BA_measure(float *pressure, float *temperature);
    float get_depth( float pressure, float pressure_reference);

#endif /* MS5837_30BA_H_ */
