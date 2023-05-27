/*
 * temp.h
 *
 *  Created on: 5 Dec 2022
 *      Author: Marcus Alexander Tjomsaas
 */

#ifndef TEMP_H_
#define TEMP_H_
    #define TSYS01_ADDRESS 0x77

    // I2C commands
    #define TSYS01_RESET_SENSOR 0x1E
    #define TSYS01_START_CONVERSION 0x48
    #define TSYS01_GET_ADC_VALUE 0X00

    #define TSYS01_GET_K4 0xA2
    #define TSYS01_GET_K3 0xA4
    #define TSYS01_GET_K2 0xA6
    #define TSYS01_GET_K1 0xA8
    #define TSYS01_GET_K0 0xAA

    #define BYTES_2 0X02

    static uint32_t k4 = 28446;
    static uint32_t k3 = 24926;
    static uint32_t k2 = 36016;
    static uint32_t k1 = 32791;
    static uint32_t k0 = 40781;

    #define K4 0X04
    #define K3 0X03
    #define K2 0X02
    #define K1 0X01
    #define K0 0X00

    void TSYS01_init();
    void TSYS01_measure(float* temperature);




#endif /* TEMP_H_ */
