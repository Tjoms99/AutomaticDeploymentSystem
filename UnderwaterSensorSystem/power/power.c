/*
 * power.c
 *
 *  Created on: 13 May 2023
 *      Author: marcus
 */
#include <MSP430.h>
#include <stdint.h>
#include "../rs485/max3471.h"


#define POWER_ENABLE BIT4

void power(uint8_t set_power){
    char* on = "on";
    char* off = "off";
    char new_line = 12;

    max3471_transmit(new_line);

    if(set_power){
        //Turn on 12V
        P1OUT |= POWER_ENABLE;

        max3471_transmit(on[0]);
        max3471_transmit(on[1]);
    } else {
        //Turn off 12V
        P1OUT &= ~POWER_ENABLE;

        max3471_transmit(off[0]);
        max3471_transmit(off[1]);
        max3471_transmit(off[2]);
    }
}

void power_init(){
  //Configure transmit enable
  P1SEL0 &= (~POWER_ENABLE); // Set P1.6 SEL for GPIO
  P1SEL1 &= (~POWER_ENABLE); // Set P1.6 SEL for GPIO
  P1DIR |= POWER_ENABLE;  // Set P1.6 as Output
  P1OUT &= ~POWER_ENABLE; // Disable 12V
}
