/*
 * temp.c
 *
 *  Created on: 5 Dec 2022
 *      Author: Marcus Alexander Tjomsaas
 */

#include <stdint.h>
#include <MSP430.h>
#include <temperature/TSYS01.h>
#include "../i2c/i2c.h"

#define TIMER_33MS 2063 //33ms

/*
static void timer_init()
{
    TA0CTL |= TASSEL_2; // set timer to system clk
    TA0CTL |= ID_2;     // prescalar 4
    TA0CTL |= MC_1;     // up mode

    TA0CCR1 = TIMER_33MS; // set compare register;
    TA0CCTL1 &= ~CCIFG;  // clear interrupt
    TA0CCTL1 |= CCIE;    // enable interrupt
    TA0CCTL1 &= ~CAP;    // compare mode
    __enable_interrupt();
}
*/
static void set_coefficients(uint8_t coefficient)
{
    if(coefficient == K4) k4 = data_in;
    if(coefficient == K3) k3 = data_in;
    if(coefficient == K2) k2 = data_in;
    if(coefficient == K1) k1 = data_in;
    if(coefficient == K0) k0 = data_in;
}

void TSYS01_init()
{
    uint32_t index;

    i2c_write(TSYS01_RESET_SENSOR, TSYS01_ADDRESS);   // reset sensor
    for(index = 0; index <10000; index++);

    i2c_write(TSYS01_GET_K4, TSYS01_ADDRESS);   // get k4
    i2c_read(BYTES_2, TSYS01_ADDRESS);
    set_coefficients(K4);

    i2c_write(TSYS01_GET_K3, TSYS01_ADDRESS);   // get k3
    i2c_read(BYTES_2, TSYS01_ADDRESS);
    set_coefficients(K3);

    i2c_write(TSYS01_GET_K2, TSYS01_ADDRESS);   // get k2
    i2c_read(BYTES_2, TSYS01_ADDRESS);
    set_coefficients(K2);

    i2c_write(TSYS01_GET_K1, TSYS01_ADDRESS);   // get k1
    i2c_read(BYTES_2, TSYS01_ADDRESS);
    set_coefficients(K1);

    i2c_write(TSYS01_GET_K0, TSYS01_ADDRESS);   // get k0
    i2c_read(BYTES_2, TSYS01_ADDRESS);
    set_coefficients(K0);

    timer_init();
}

/*
static void wait_for_conversion()
{
    TA0CCTL1 |= CCIE;           // enable interrupt

    //set register to trigger 33ms into the future
    TA0CCR1 = TA0R + TIMER_33MS; // set compare register
    if(TA0R + TIMER_33MS > TA0CCR0) TA0CCR1 = TIMER_33MS - (TA0CCR0 - TA0R); // in case of overflow when setting register

    while((TA0IV & 0X02) == 0);
}
*/
static void read_adc()
{

    i2c_write(TSYS01_START_CONVERSION, TSYS01_ADDRESS);   // start conversion
    wait_for_conversion();
    i2c_write(TSYS01_GET_ADC_VALUE,TSYS01_ADDRESS);   // read adc temp value
    i2c_read(BYTES_2, TSYS01_ADDRESS);

}

static void TSYS01_calculate_temperature(float *temperature)
{
    static float adc = 9378708/256;

    read_adc();
    adc = data_in;

    float k4_t = -1 * (2.0f * k4 / 1000000000000000000000.0f * adc * adc * adc * adc);
    float k3_t =      (4.0f * k3 / 10000000000000000.0f * adc * adc * adc);
    float k2_t = -1 * (2.0f * k2 / 100000000000.0f * adc * adc);
    float k1_t =      (1.0f * k1 / 1000000.0f  * adc);
    float k0_t = -1 * (1.5f * k0 / 100.0f);

    *temperature = k4_t + k3_t + k2_t + k1_t + k0_t;
}

void TSYS01_measure(float* temperature)
{
    TSYS01_calculate_temperature(temperature);
}

/*
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer_A_CCR1_ISR(void)
{
    switch(TA0IV){
    case 0x02:
        //TA0CCTL1 &= ~CCIE;    // disable interrupt
        //TA0CCR1 = TA0R - 1;
        break;
    default:
        break;
    }
}
*/
