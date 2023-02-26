/*
 * uart.c
 *
 *  Created on: 31 Jan 2023
 *      Author: marcus
 */

#include "uart.h"
#include <stdint.h>
#include <msp430.h>

#define RS485_TX BIT4 //BIT4
#define RS485_RX BIT5 //BIT5
#define RS485_DE_NRE BIT6

//See datasheet Table 15-4
void uart_set_mode(uint8_t mode)
{

    switch(mode){
    case 0://9600; TX error: -0.5 0.6; RX error: -0.9 1.2
        UCA0BR0 = 104;
        UCA0MCTLW = 0x0100;
        break;
    case 1:  //115200; TX error: -7.8 6.4; RX error: -9.7 16.1
        UCA0BR0 = 8;
        UCA0MCTLW = 0xD600; //0x0600
        break;

    }
}

void uart_init()
{

    //Pinout

    P1SEL0 &= (~RS485_DE_NRE); // Set P1.6 SEL for GPIO
    P1SEL1 &= (~RS485_DE_NRE); // Set P1.6 SEL for GPIO
    P1DIR |= RS485_DE_NRE;  // Set P1.6 as Output
    P1OUT &= ~RS485_DE_NRE; // Recieve enabled



  // Configure UART
  UCA0CTLW0 |= UCSWRST;         // SW reset
  UCA0CTLW0 |= UCSSEL__SMCLK;   // SMCLK = BRCLK (115200 BAUD)
  uart_set_mode(0);


  // Configure UART pins
  P1SEL1 &= ~(RS485_TX | RS485_RX);
  P1SEL0 |= RS485_TX | RS485_RX;

  PM5CTL0 &= ~LOCKLPM5;  // turn on I/O

  UCA0CTLW0 &= ~UCSWRST; // Remove software reset

  // Enable the UART receive interrupt
  UCA0IE |= UCRXIE;
  __enable_interrupt();   // enable maskables

}

void uart_transmit(char data)
{
    int ii = 0;
  P1OUT |= RS485_DE_NRE; // Set transmitt enable
  // Wait for the transmit buffer to be ready
  while (!(UCA0IFG & UCTXIFG));

  // Transmit the data
  UCA0TXBUF = data;
  for(ii = 0; ii < 1000; ii++);

  P1OUT &= ~RS485_DE_NRE; // Set recieve enable

}

void uart_recieve(char *data)
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
    if(UCA0RXBUF > 0x00) //A0
    {
        uart_transmit(UCA0RXBUF);

    }
}
