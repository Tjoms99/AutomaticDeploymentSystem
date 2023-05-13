/*
 * uart.c
 *
 *  Created on: 31 Jan 2023
 *      Author: marcus
 */

#include <stdint.h>
#include <msp430.h>
#include <rs485/max3471.h>

#define TX BIT3
#define RX BIT2
#define ENABLE BIT4

//See datasheet Table 15-4
void icl3221_set_mode(uint8_t mode)
{

    switch(mode){
    case 0://9600; TX error: -0.5 0.6; RX error: -0.9 1.2
        UCA1BR0 = 104;
        UCA1MCTLW = 0x0100;
        break;
    case 1:  //115200; TX error: -7.8 6.4; RX error: -9.7 16.1
        //CAUSES JITTER
        UCA1BR0 = 8;
        UCA1MCTLW = 0xD600;
        break;

    }
}

void icl3221_power(uint8_t on){

    //Turn off RS232
    P4OUT &= ~ENABLE;

    //Turn on RS232
    if(on){
        P4OUT |= ENABLE;
    }
}

void icl3221_init()
{

  //Configure transmit enable
  P4SEL0 &= (~ENABLE); // Set P1.6 SEL for GPIO
  P4SEL1 &= (~ENABLE); // Set P1.6 SEL for GPIO
  P4DIR |= ENABLE;  // Set P1.6 as Output
  P4OUT &= ~ENABLE; // Disable device

  // Configure Tx and Rx
  P4SEL1 &= ~(TX | RX);
  P4SEL0 |= TX | RX;

  // Configure UART settings
  UCA1CTLW0 |= UCSWRST;         // SW reset
  UCA1CTLW0 |= UCSSEL__SMCLK;   // SMCLK = BRCLK (115200 BAUD)
  icl3221_set_mode(0);

  PM5CTL0 &= ~LOCKLPM5;  // turn on I/O
  UCA1CTLW0 &= ~UCSWRST; // Remove software reset

  // Receive interrupt enable
  UCA1IE |= UCRXIE;
  __enable_interrupt();

}

void icl3221_transmit(char data)
{
  int ii = 0;
  //Transmit mode
  UCA1IE &= ~UCRXIE;

  // Wait for the transmit buffer to be ready
  while (!(UCA1IFG & UCTXIFG));

  // Transmit the data
  UCA1TXBUF = data;
  for(ii = 0; ii < 1000; ii++);

  //Recieve mode
  P1OUT &= ~ENABLE;
  UCA1IE |= UCRXIE;
}

void icl3221_recieve(char *data)
{
  // Wait for a received character
  while (!(UCA1IFG & UCRXIFG));

  // Return the received character
  *data = (char) UCA1RXBUF;
}

// Interrupt Service Routine for UART receive
#pragma vector=USCI_A1_VECTOR //A1
__interrupt void RS232_ISR(void)
{
    char data = '0';
    if(UCA1RXBUF > 0x00) //A1
    {
        data = UCA1RXBUF;
        icl3221_transmit(data);

    }
}
