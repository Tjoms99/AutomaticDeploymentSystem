
#include <MSP430.h>
#include <stdint.h>

//#include <pressure/MS5837_30BA.h>
//#include <temperature/TSYS01.h>
//#include <i2c/i2c.h>
#include <uart/uart.h>

#define TIMER_1S 62500

uint8_t TEMPFG = 1;

void timer_1_init(){
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
    static float MS5837_30BA_pressure = 0;
    static float MS5837_30BA_temperature = 0;
    static float TSYS01_temperature = 0;

    static float pressure_reference = 0;
    volatile static float depth = 0;




    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    timer_1_init();
    uart_init();

    int i;
    while(1)
    {
       char data = 'a';
       for(i = 0; i < 10000; i++);
    }
}



#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
    TEMPFG = 1;
    TA0CCTL0 &= ~CCIFG;  // clear interrupt
}
