
#include <MSP430.h>
#include <stdint.h>
#include "i2c/i2c.h"
#include "temperature/temp.h"

//Note for next time:
//  Add pressure driver
//  Add timer interval of 1 sec to sample temperature and pressure

uint8_t TEMPFG = 1;

void init_timer1(){
    TA0CCR0 = 62500;    // set the compare reg
    TA0CTL = TASSEL_2;  // set timer to system clk
    TA0CTL |= ID_2;     // prescalar 4
    TA0CTL |= MC_1;     // up mode

    TA0CCTL0 &= ~CCIFG;  // clear interrupt
    TA0CCTL0 |= CCIE;    // enable interrupt
    TA0CCTL0 &= ~CAP;    // compare mode
    __enable_interrupt();
}

int main(void)
{
    volatile static float temperature = 0;


    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    init_timer1();
    init_i2c();
    init_temp();

    while(1)
    {


       if(TEMPFG == 1){
           temperature = get_temperature();
           TEMPFG = 0;

       }
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
    TEMPFG = 1;
    TA0CCTL0 &= ~CCIFG;  // clear interrupt
}
