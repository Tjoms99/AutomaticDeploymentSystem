/*
 * uart.c
 *
 *  Created on: 31 Jan 2023
 *      Author: marcus
 */

#include <stdint.h>
#include <msp430.h>
#include <rs485/max3471.h>

#define TX BIT4
#define RX BIT5
#define TX_EN BIT6

//See datasheet Table 15-4
void max3471_set_mode(uint8_t mode)
{

    switch(mode){
    case 0://9600; TX error: -0.5 0.6; RX error: -0.9 1.2
        UCA0BR0 = 104;
        UCA0MCTLW = 0x0100;
        break;
    case 1:  //115200; TX error: -7.8 6.4; RX error: -9.7 16.1
        //CAUSES JITTER
        UCA0BR0 = 8;
        UCA0MCTLW = 0xD600;
        break;

    }
}

void max3471_init()
{

  //Configure transmit enable
  P1SEL0 &= (~TX_EN); // Set P1.6 SEL for GPIO
  P1SEL1 &= (~TX_EN); // Set P1.6 SEL for GPIO
  P1DIR |= TX_EN;  // Set P1.6 as Output
  P1OUT &= ~TX_EN; // Recieve enabled

  // Configure Tx and Rx
  P1SEL1 &= ~(TX | RX);
  P1SEL0 |= TX | RX;

  // Configure UART settings
  UCA0CTLW0 |= UCSWRST;         // SW reset
  UCA0CTLW0 |= UCSSEL__SMCLK;   // SMCLK = BRCLK (115200 BAUD)
  max3471_set_mode(0);

  PM5CTL0 &= ~LOCKLPM5;  // turn on I/O
  UCA0CTLW0 &= ~UCSWRST; // Remove software reset

  // Receive interrupt enable
  UCA0IE |= UCRXIE;
  __enable_interrupt();

}

void max3471_transmit(char data)
{
  int ii = 0;
  //Transmit mode
  UCA0IE &= ~UCRXIE;
  P1OUT |= TX_EN;

  // Wait for the transmit buffer to be ready
  while (!(UCA0IFG & UCTXIFG));

  // Transmit the data
  UCA0TXBUF = data;
  for(ii = 0; ii < 1000; ii++);

  //Recieve mode
  P1OUT &= ~TX_EN;
  UCA0IE |= UCRXIE;
}

void max3471_recieve(char *data)
{
  // Wait for a received character
  while (!(UCA0IFG & UCRXIFG));

  // Return the received character
  *data = (char) UCA0RXBUF;
}

// Interrupt Service Routine for UART receive
#pragma vector=USCI_A0_VECTOR //A0
__interrupt void UART_ISR(void)
{
    char data = '0';
    if(UCA0RXBUF > 0x00) //A0
    {
        data = UCA0RXBUF;
        max3471_transmit(data);

    }
}
