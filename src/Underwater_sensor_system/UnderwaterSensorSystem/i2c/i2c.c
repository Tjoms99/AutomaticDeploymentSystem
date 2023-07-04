/*
 * i2c.c
 *
 *  Created on: 5 Dec 2022
 *      Author: Marcus Alexander Tjomsaas
 */

#include <MSP430.h>
#include <stdint.h>
#include "i2c.h"

#define SDA_PIN BIT2
#define SCL_PIN BIT3

void i2c_init()
{
        UCB0CTLW0 |= UCSWRST; // SW RST

        UCB0CTLW0 |= UCSSEL_3; // choose SMCLK
        UCB0BRW = 16;          // set prescalar to

        UCB0CTLW0 |= UCMODE_3; // put in I2C mode
        UCB0CTLW0 |= UCMST;    // set as master
        UCB0CTLW0 |= UCTR;     // put into TX mode (WRITE)
                               // set slave address RTC = 0x77

        UCB0CTLW1 |= UCASTP_2; // auto STOP mode
        UCB0TBCNT = 1;         // count = 1 byte

        //-- setup ports; SEL1 = 0; SEL0 = 1
        P1SEL1 &= ~SCL_PIN;
        P1SEL0 |= SCL_PIN; // P1.3 = SCL

        P1SEL1 &= ~SDA_PIN;
        P1SEL0 |= SDA_PIN; // P1.2 = SDA

        // SYSCFG3 |= BIT4;
        PM5CTL0 &= ~LOCKLPM5; // turn on I/O

        UCB0CTLW0 &= ~UCSWRST; // set in operational mode

        //-- enable B1 TX0 IRQ
        UCB0IE |= UCTXIE0; // Tx IRQ
        UCB0IE |= UCRXIE0; // Rx IRQ

        __enable_interrupt(); // enable maskables
}

void i2c_write(uint8_t cmd, uint8_t address)
{
        // Transmit Data to slave with a WRITE message
        UCB0I2CSA = address; // set slave address
        UCB0TBCNT = 1;       // count = 1 byte
        i2c_data_out = cmd;

        UCB0CTLW0 |= UCTR;    // Put into Tx mode
        UCB0CTLW0 |= UCTXSTT; // manually start message (START)

        while ((UCB0IFG & UCSTPIFG) == 0)
                ;

        UCB0IFG &= ~UCSTPIFG; // clear the stop flag
}

void i2c_read(uint8_t bytes_to_read, uint8_t address)
{
        // Receive Data from slave with a READ message
        UCB0TBCNT = bytes_to_read;
        UCB0I2CSA = address; // set slave address

        UCB0CTLW0 &= ~UCTR;   // Put into Rx mode
        UCB0CTLW0 |= UCTXSTT; // manually start message (START)

        while ((UCB0IFG & UCSTPIFG) == 0)
                ;

        UCB0IFG &= ~UCSTPIFG; // clear the stop flag
}

//-- ISRs
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void)
{
#define RXIF0 0x16
#define TXIF0 0x18

        switch (UCB0IV)
        {
        case RXIF0: // read Rx buffer

                i2c_data_in = i2c_data_in << 8 | UCB0RXBUF;
                break;

        case TXIF0: // set Tx buffer
                UCB0TXBUF = i2c_data_out;

                i2c_data_in = 0;
                break;

        default:
                break;
        }
}
