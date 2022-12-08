/*
 * temp.h
 *
 *  Created on: 5 Dec 2022
 *      Author: Marcus Alexander Tjomsaas
 */

#ifndef TEMP_H_
#define TEMP_H_

    #define TEMPERATURE_ADDRESS 0x77

    #define RESET_SENSOR 0x1E
    #define START_CONVERSION 0x48
    #define GET_TEMP_VALUE 0X00

    #define READ_2_BYTES 0X02
    #define READ_3_BYTES 0X03

    #define GET_K4 0xA2
    #define GET_K3 0xA4
    #define GET_K2 0xA6
    #define GET_K1 0xA8
    #define GET_K0 0xAA

    #define K4 0X04
    #define K3 0X03
    #define K2 0X02
    #define K1 0X01
    #define K0 0X00


    volatile static uint32_t k4 = 28446;
    volatile static uint32_t k3 = 24926;
    volatile static uint32_t k2 = 36016;
    volatile static uint32_t k1 = 32791;
    volatile static uint32_t k0 = 40781;

    void init_temp();
    double get_temperature();




#endif /* TEMP_H_ */
