/*
 * pressure.h
 *
 *  Created on: 7 Dec 2022
 *      Author: marcu
 */

#ifndef PRESSURE_PRESSURE_H_
#define PRESSURE_PRESSURE_H_

    #define PRESSURE_ADDRESS 0x76

    #define RESET_SENSOR 0x1E
    #define GET_ADC_VALUE 0x00
    #define PROM_READ 0xA0
    #define START_CONVERTION_D1_4096 0x48
    #define START_CONVERTION_D2_4096 0x58

    #define BYTES_2 0X02
    #define BYTES_3 0X03

    #define POW_2_7 1<<7
    #define POW_2_8 1<<8
    #define POW_2_15 1<<15
    #define POW_2_16 1<<16
    #define POW_2_23 1<<23

    const float Pa = 100.0f;
    const float bar = 0.001f;
    const float mbar = 1.0f;
    uint16_t fluidDensity = 1029;

    static  uint16_t constants[8];
    static  uint32_t D1_pres, D2_temp;
    static  int32_t TEMP;
    static  int32_t P;
    static  uint8_t _model;



    void init_pressure(void);
    uint16_t get_temperature_p();
    uint16_t get_pressure();
    float get_depth();
    float get_altitude();
    static void calculate();
    static uint8_t crc4(uint16_t n_prom[]);


#endif /* PRESSURE_PRESSURE_H_ */
