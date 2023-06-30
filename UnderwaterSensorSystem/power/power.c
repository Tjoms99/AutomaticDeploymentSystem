/*
 * power.c
 *
 *  Created on: 13 May 2023
 *      Author: marcus
 */
#include <MSP430.h>
#include <stdint.h>
#include "../rs485/max3471.h"
#include "../rs232/icl3221.h"

#define VOLT12_ENABLE BIT4
char *on = "on";
char *off = "off";
char new_line = 12;

void power(uint8_t set_power)
{
    max3471_transmit(new_line);
    icl3221_transmit(new_line);

    if (set_power)
    {
        // Turn on 12V
        P1OUT |= VOLT12_ENABLE;


        max3471_transmit(on[0]);
        max3471_transmit(on[1]);

        icl3221_transmit(on[0]);
        icl3221_transmit(on[1]);
    }
    else
    {
        // Turn off 12V
        P1OUT &= ~VOLT12_ENABLE;

        max3471_transmit(off[0]);
        max3471_transmit(off[1]);
        max3471_transmit(off[2]);

        icl3221_transmit(off[0]);
        icl3221_transmit(off[1]);
        icl3221_transmit(off[2]);
    }
}

void power_init()
{
    // Configure transmit enable
    P1SEL0 &= (~VOLT12_ENABLE); // Set P1.6 SEL for GPIO
    P1SEL1 &= (~VOLT12_ENABLE); // Set P1.6 SEL for GPIO
    P1DIR |= VOLT12_ENABLE;     // Set P1.6 as Output
    P1OUT &= ~VOLT12_ENABLE;    // Disable 12V
}
