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
#define POWER_ON BIT4

// See datasheet Table 15-4
void icl3221_set_mode(uint8_t mode)
{
  switch (mode)
  {
  case 0: // BW: 56 000 @ 16MHz
    UCA1BRW = 285;
    UCA1MCTLW = 0xBB00;
    break;
  case 1: // BW: 9600 @ 16MHz
    UCA1BRW = 1666;
    UCA1MCTLW = 0xD600;
    break;
  }
}

void icl3221_turn_on()
{
  // Turn on RS232
  P4OUT |= POWER_ON;
}

void icl3221_turn_off()
{
  // Turn off RS232
  P4OUT &= ~POWER_ON;
}

void icl3221_init()
{

  // Configure power_on
  P4SEL0 &= (~POWER_ON);
  P4SEL1 &= (~POWER_ON);
  P4DIR |= POWER_ON;  // Set as output
  P4OUT &= ~POWER_ON; // Disable device

  // Configure Tx and Rx
  P4SEL1 &= ~(TX | RX);
  P4SEL0 |= TX | RX;

  // Configure UART settings
  UCA1CTLW0 |= UCSWRST;       // SW reset
  UCA1CTLW0 |= UCSSEL__SMCLK; // SMCLK = BRCLK (115200 BAUD)
  icl3221_set_mode(1);

  PM5CTL0 &= ~LOCKLPM5;  // turn on I/O
  UCA1CTLW0 &= ~UCSWRST; // Remove software reset

  // Receive interrupt enable
  UCA1IE |= UCRXIE;
  __enable_interrupt();
}

void icl3221_transmit(char data)
{
  int ii = 0;
  // Transmit mode
  UCA1IE &= ~UCRXIE;

  // Wait for the transmit buffer to be ready
  while (!(UCA1IFG & UCTXIFG))
    ;

  // Transmit the data
  UCA1TXBUF = data;
  for (ii = 0; ii < 1000; ii++)
    ;

  // Recieve mode
  UCA1IE |= UCRXIE;
}

void icl3221_recieve(char *data)
{
  // Wait for a received character
  while (!(UCA1IFG & UCRXIFG))
    ;

  // Return the received character
  *data = (char)UCA1RXBUF;
}
