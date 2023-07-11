/*
 * power.c
 *
 *  Created on: 13 May 2023
 *      Author: marcus
 */
#include <MSP430.h>
#include <stdint.h>
#include "power.h"

#define VOLT12_ENABLE BIT4

void power(uint8_t on)
{
    if (on)
    {
        P1OUT |= VOLT12_ENABLE;
    }
    else
    {
        P1OUT &= ~VOLT12_ENABLE;
    }
}

void power_init()
{
    P1SEL0 &= (~VOLT12_ENABLE); // Set P1.6 SEL for GPIO
    P1SEL1 &= (~VOLT12_ENABLE); // Set P1.6 SEL for GPIO
    P1DIR |= VOLT12_ENABLE;     // Set P1.6 as Output
    P1OUT &= ~VOLT12_ENABLE;    // Disable 12V
}
