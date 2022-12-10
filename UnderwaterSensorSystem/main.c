
#include <MSP430.h>
#include <stdint.h>
#include <temperature/TSYS01.h>
#include "i2c/i2c.h"
#include "pressure/pressure.h"

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
    volatile static float TSYS01_temperature = 0;

    volatile static float pressure = 0;
    volatile static float depth = 0;




    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    init_timer1();
    init_i2c();
    TSYS01_init();
    init_pressure();

    while(1)
    {

        uint32_t i = 0;

       if(TEMPFG == 1){
           TSYS01_measure_temperature(&TSYS01_temperature);

           measure();
           temperature1 =  get_temperature_p() ;
           pressure = get_pressure(Pa);
           depth = get_depth()*100.0f;
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
