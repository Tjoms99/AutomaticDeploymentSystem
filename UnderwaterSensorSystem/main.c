
#include <MSP430.h>
#include <rs485/max3471.h>
#include <rs232/icl3221.h>
#include <clock/clock.h>
#include <power/power.h>

#include <stdint.h>


#include <pressure/MS5837_30BA.h>
#include <temperature/TSYS01.h>
#include <i2c/i2c.h>
#include <storage/memory.h>
#include <library/conversions.h>
#define TIMER_1S 32768

volatile uint8_t SYSTEM_FLAG = 1;
uint8_t control_reg = 0xFF;



void update_system_flags(char data){
    if(data == '0') SYSTEM_FLAG ^= POWER_ENABLE;
    if(data == '1') SYSTEM_FLAG ^= RS232_ENABLE;
    if(data == '2') SYSTEM_FLAG ^= CUSTOM_TIME;
    if(data == '3') SYSTEM_FLAG ^= SINGLE_MODE;
    if(data == '4') SYSTEM_FLAG ^= CONTINUOUS_MODE;
    if(data == '7') SYSTEM_FLAG = 0; //Reset
}

static void timer_init(){

    TB0CTL |= TBCLR;        // reset TB0
    TB0CTL |= TBSSEL__ACLK; // ACLK
    TB0CTL |= MC__UP;       // up mode

    TB0CCR0 = TIMER_1S;        // set the compare register

    TB0CCTL0 |= CCIE;       // enable interrupt
    __enable_interrupt();
    TB0CCTL0 &= ~CCIFG;     // clear interrupt

}

void print_current_temperature(){
    float temperature = 0.0;
    char buffer[6];

    get_temperature_current_register(&temperature);

    float_to_char_array(temperature, buffer);
    max3471_transmit(12);
    max3471_transmit(buffer[0]);
    max3471_transmit(buffer[1]);
    max3471_transmit(buffer[2]);
    max3471_transmit(buffer[3]);
    max3471_transmit(buffer[4]);
    max3471_transmit(buffer[5]);
}

//Sample pressure and temperature once
void single_mode(){
    static float TSYS01_temperature = 0;

    TSYS01_measure(&TSYS01_temperature);
    set_temperature_current_register(TSYS01_temperature);
}

//Sample pressure and temperature once
void continuous_mode(){
    static float TSYS01_temperature = 0;

    TSYS01_measure(&TSYS01_temperature);
    set_temperature_current_register(TSYS01_temperature);

}


int main(void)
{
    static float MS5837_30BA_pressure = 0;
    static float MS5837_30BA_temperature = 0;

    static float pressure_reference = 0;
    volatile static float depth = 0;

    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    clock_init_16mhz();
    timer_init();
    i2c_init();

    power_init();
    max3471_init();
    max3471_set_mode(0);

    icl3221_init();
    icl3221_set_mode(1);
    icl3221_turn_on();


    TSYS01_init();

    set_control_register(control_reg);
    get_control_register(&control_reg);

    while(1)
    {


      if(SYSTEM_FLAG & BIT7) {
          if(SYSTEM_FLAG & POWER_ENABLE){
                power(0xFF);
            } else {
                power(0x00);
            }

            if(SYSTEM_FLAG & SINGLE_MODE){
                single_mode();
            }

            if(SYSTEM_FLAG & CONTINUOUS_MODE){
                print_current_temperature();
            }

            SYSTEM_FLAG &= ~BIT7;
      }


     //RS485 Rx does not work when LPMx > 1
      __bis_SR_register(LPM1_bits | GIE); //Enter low-power mode
      __no_operation();

    }
}



#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
    __bic_SR_register_on_exit(LPM1_bits);

    SYSTEM_FLAG |= BIT7;
    TBCCTL0 &= ~CCIFG;  // clear interrupt
}

// Interrupt Service Routine for UART receive
#pragma vector=USCI_A0_VECTOR
__interrupt void RS485_ISR(void)
{
    __bic_SR_register_on_exit(LPM1_bits);

    char data = UCA0RXBUF;
    max3471_transmit(data);

    update_system_flags(data);

}

// Interrupt Service Routine for UART receive
#pragma vector=USCI_A1_VECTOR
__interrupt void RS232_ISR(void)
{
    __bic_SR_register_on_exit(LPM1_bits);

    char data = UCA1RXBUF;
    icl3221_transmit(data);

    update_system_flags(data);

}


