/*
 * pressure.h
 *
 *  Created on: 7 Dec 2022
 *      Author: marcu
 */

#ifndef PRESSURE_H_
#define PRESSURE_H_

    #define PRESSURE_ADDRESS 0x76

    #define TSYS01_RESET_SENSOR 0x1E
    #define GET_ADC_VALUE_P 0x00
    #define PROM_READ 0xA0
    #define START_CONVERTION_D1_4096 0x48
    #define START_CONVERTION_D2_4096 0x58

    #define BYTES_2 0X02
    #define BYTES_3 0X03

    #define SEAWATER_DENSITY 1029   //kg/m^3
    #define FRESHWATER_DENSITY 1000 //kg/m^3

    #define Pa 100.0f
    #define bar 0.001f
    #define mbar 1.0f
    static float start_Pa = 0;

    static  uint16_t coefficients[8];
    static  uint32_t D1_pres, D2_temp;
    static  int32_t TEMP;
    static  int32_t P;



    void init_pressure(void);
    float get_temperature_p();
    float get_pressure(float conversion);
    float get_depth();
    static void calculate();
    static uint8_t crc4(uint16_t n_prom[]);


#endif /* PRESSURE_H_ */
