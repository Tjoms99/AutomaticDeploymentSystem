/*
 * i2c.c
 *
 *  Created on: 5 Dec 2022
 *      Author: Marcus Alexander Tjomsaas
 */

#include <MSP430.h>
#include <stdint.h>
#include "i2c.h"


void init_i2c()
{
        UCB1CTLW0 |= UCSWRST;   // SW RST

        UCB1CTLW0 |= UCSSEL_3;  // choose SMCLK
        UCB1BRW = 10;           // set prescalar to 10

        UCB1CTLW0 |= UCMODE_3;  // put in I2C mode
        UCB1CTLW0 |= UCMST;     // set as master
        UCB1CTLW0 |= UCTR;      // put into TX mode (WRITE)
               // set slave address RTC = 0x77

        UCB1CTLW1 |= UCASTP_2;  // auto STOP mode
        UCB1TBCNT = 1;          // count = 1 byte

        //-- setup ports; SEL1 = 0; SEL0 = 1
        P4SEL1 &= ~BIT3;        //P1.3 = SCL
        P4SEL0 |= BIT3;

        P4SEL1 &= ~BIT4;        //P1.2 = SDA
        P4SEL0 |= BIT4;

        SYSCFG3 |= BIT4;
        PM5CTL0 &= ~LOCKLPM5;  // turn on I/O

        UCB1CTLW0 &= ~UCSWRST;  //set in operational mode

        //-- enable B1 TX0 IRQ
        UCB1IE |= UCTXIE0;      // Tx IRQ
        UCB1IE |= UCRXIE0;      // Rx IRQ

        __enable_interrupt();   // enable maskables
}

void i2c_write(uint8_t cmd, uint8_t address)
{
        //Transmit Data to slave with a WRITE message
        UCB1I2CSA = address;    // set slave address
        UCB1TBCNT = 1;          // count = 1 byte
        data_out = cmd;

        UCB1CTLW0 |= UCTR;      // Put into Tx mode
        UCB1CTLW0 |= UCTXSTT;   // manually start message (START)

        while((UCB1IFG & UCSTPIFG) == 0);
        UCB1IFG &= ~UCSTPIFG;   // clear the stop flag
}

void i2c_read(uint8_t bytes_to_read)
{
        //Receive Data from slave with a READ message
        UCB1TBCNT = bytes_to_read;
        UCB1CTLW0 &= ~UCTR;         // Put into Rx mode
        UCB1CTLW0 |= UCTXSTT;       // manually start message (START)

        while((UCB1IFG & UCSTPIFG) == 0);
        UCB1IFG &= ~UCSTPIFG;       // clear the stop flag
}

//-- ISRs
#pragma vector = EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void)
{
        static uint8_t byte_pos = 0;

        switch(UCB1IV)
        {
            case RXIF0: // read Rx buffer

                data_in = data_in << 8*byte_pos | UCB1RXBUF;
                byte_pos++;
                break;


            case TXIF0: // set Tx buffer
                UCB1TXBUF = data_out;

                byte_pos = 0;
                data_in = 0;
                break;

            default:
                break;
        }
}

