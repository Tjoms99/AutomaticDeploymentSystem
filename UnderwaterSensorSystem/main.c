
#include <MSP430.h>
#include <rs485/max3471.h>
#include <rs232/icl3221.h>
#include <clock/clock.h>
#include <power/power.h>

#include <stdint.h>

//#include <pressure/MS5837_30BA.h>
#include <temperature/TSYS01.h>
#include <i2c/i2c.h>

#define TIMER_1S 32768

uint8_t TEMPFG = 1;

static void timer_init(){

    TB0CTL |= TBCLR;        // reset TB0
    TB0CTL |= TBSSEL__ACLK; // ACLK
    TB0CTL |= MC__UP;       // up mode

    TB0CCR0 = TIMER_1S;        // set the compare register

    TB0CCTL0 |= CCIE;       // enable interrupt
    __enable_interrupt();
    TB0CCTL0 &= ~CCIFG;     // clear interrupt

}



int main(void)
{
    static float MS5837_30BA_pressure = 0;
    static float MS5837_30BA_temperature = 0;
    static float TSYS01_temperature = 0;

    static float pressure_reference = 0;
    volatile static float depth = 0;



    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    //clock_init_1mhz();
    //clock_init_8mhz();
    clock_init_16mhz();

    init_i2c();

    timer_init();

    power_init();
    max3471_init();
    max3471_set_mode(0);

    icl3221_init();
    icl3221_set_mode(1);
    icl3221_turn_on();


    TSYS01_init();


    while(1)
    {


      if(TEMPFG){
          power(0xFF);
          icl3221_transmit('a');
          TSYS01_measure(&TSYS01_temperature);
          TEMPFG = 0;
          __bis_SR_register(LPM3_bits | GIE);     // Enter LPM3
          __no_operation();                       // For debug
      }


/*
       power(0xFF);
       icl3221_transmit('b');
       __delay_cycles(16000000*1);

*/

    }
}



#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
    __bic_SR_register_on_exit(LPM3_bits);

    TEMPFG = 1;
    TBCCTL0 &= ~CCIFG;  // clear interrupt
}

