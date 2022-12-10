
#include <MSP430.h>
#include <stdint.h>
#include "i2c/i2c.h"
#include "temperature/temp.h"

//Note for next time:
//  Add pressure driver
//  Add timer interval of 1 sec to sample temperature and pressure

#define TIMER_1S 62500
uint8_t TEMPFG = 1;

void init_timer1(){
    TA0CCR0 = TIMER_1S; // set the compare register
    TA0CTL |= TASSEL_2; // set timer to system clk
    TA0CTL |= ID_2;     // prescalar 4
    TA0CTL |= MC_1;     // up mode

    TA0CCTL0 &= ~CCIFG; // clear interrupt
    TA0CCTL0 |= CCIE;   // enable interrupt
    TA0CCTL0 &= ~CAP;   // compare mode
    __enable_interrupt();
}

int main(void)
{
    volatile static float temperature1 = 0;
    volatile static float temperature2 = 0;

    volatile static float pressure = 0;
    volatile static float depth = 0;




    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    init_timer1();
    init_i2c();
    init_temperature();
    init_pressure();

    while(1)
    {

        uint32_t i = 0;

       //if(TEMPFG == 1){
           measure();
           temperature1 =  get_temperature_p() / 100.0f;
           pressure = get_pressure();
           temperature2 = get_temperature();

           TEMPFG = 0;
           for(i = 0; i <60000; i++);
       //}
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
    TEMPFG = 1;
    TA0CCTL0 &= ~CCIFG;  // clear interrupt
}
