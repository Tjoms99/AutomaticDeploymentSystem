#include <MSP430.h>
#include <rs485/max3471.h>
#include <rs485/print.h>
#include <rs232/icl3221.h>
#include <clock/clock.h>
#include <clock/watchdog.h>
#include <power/power.h>
#include <sensors/pressure/MS5837_30BA.h>
#include <sensors/sensors.h>
#include <i2c/i2c.h>
#include <storage/memory.h>
#include <stdint.h>
#include "state_machine.h"

// Initializes value on code load.
// Retains value during reset or power off.
// Usefull to keep state even though the watchdog has kicked or unexpected power loss.
#pragma PERSISTENT(SYSTEM_FLAG)
uint8_t SYSTEM_FLAG = 0;

#pragma PERSISTENT(SYSTEM_FLAG)
uint16_t SYSTEM_LOOP_TIME_SECONDS = 1;

#pragma PERSISTENT(SYSTEM_FLAG)
uint16_t SYSTEM_TIMER_INTERRUPT_COUNTER = 0;

#define TIMER_1S 0x7FFF /// 7FFF 1s

static volatile uint8_t time_char_recieved = 0;
static volatile char time_char[4];

/**
 * SYSTEM FLAGS
 *  0 - Sample temperature, pressure and calculate depth.
 *  1 - Set custom time interval used for the state machine loop.
 *  2 - Set new relative depth = 0, used in depth calculation.
 *  3 - RS232 enable
 *  4 - Power enable: 12V
 *  5 - Print current temperature
 *  6 - Print temperature registers
 *  7 - Reset: Set all flags to 0
 *
 */
void state_machine_update_flag(char data)
{
    // ADD TO CUSTOM TIME

    // SET FLAG
    if (data == 'a')
    {
        SYSTEM_FLAG ^= SENSOR_SAMPLE;
    }
    else if (data == 'b' && SYSTEM_FLAG & CUSTOM_TIME)
    {
        SYSTEM_FLAG ^= CUSTOM_TIME;
        state_machine_set_loop_interval();
    }
    else if (data == 'b')
    {
        SYSTEM_FLAG ^= CUSTOM_TIME;
    }
    else if (SYSTEM_FLAG & CUSTOM_TIME)
    {
        time_char[time_char_recieved++] = data;
    }
    else if (data == 'c')
    {
        SYSTEM_FLAG ^= DEPTH_ZERO;
    }
    else if (data == 'd')
    {
        SYSTEM_FLAG ^= RS232_ENABLE;
    }
    else if (data == 'e')
    {
        SYSTEM_FLAG ^= VOLT12_ENABLE;
    }
    else if (data == 'f')
    {
        state_machine_reset();
    }
}

void state_machine_get_flag(uint8_t *flag)
{
    *flag = SYSTEM_FLAG;
}

void state_machine_set_flag(uint8_t flag)
{
    SYSTEM_FLAG |= flag;
}

void state_machine_remove_flag(uint8_t flag)
{
    SYSTEM_FLAG &= ~flag;
}

static void check_system_loop_time()
{
    if (++SYSTEM_TIMER_INTERRUPT_COUNTER >= SYSTEM_LOOP_TIME_SECONDS)
    {
        SYSTEM_FLAG |= SYSTEM_ON;
        SYSTEM_TIMER_INTERRUPT_COUNTER = 0;
    }
}

void state_machine_set_loop_interval()
{
    static const uint8_t ASCII_CHAR_0 = 48;
    uint16_t seconds = 0;
    uint8_t total_time_char_recieved = time_char_recieved;


    for(time_char_recieved; time_char_recieved > 0; time_char_recieved--){
        uint16_t multiplier = 1;

        if(time_char_recieved != 1) {
            multiplier = 10 * (time_char_recieved -1);
        }

        uint16_t digit = (uint16_t)time_char[total_time_char_recieved - time_char_recieved] - ASCII_CHAR_0;
        digit *= multiplier;

        seconds += digit;
    }


    if (time_char_recieved == 0)
    {
        SYSTEM_LOOP_TIME_SECONDS = seconds;
        SYSTEM_FLAG &= ~CUSTOM_TIME;
    }
}

void state_machine_loop()
{

    if (SYSTEM_FLAG & SYSTEM_ON)
    {
        max3471_transmit('b');
        max3471_transmit('\n');

        UCA0IE &= ~UCRXIE; // disable RX UART interrupt

        // Check flags
        SYSTEM_FLAG &SENSOR_SAMPLE ? sensors_sample_and_print() : __no_operation;

        // SYSTEM_FLAG &CUSTOM_TIME ? state_machine_set_loop_interval() : __no_operation;

        SYSTEM_FLAG &DEPTH_ZERO ? sensors_set_depth_zero() : __no_operation;

        SYSTEM_FLAG &RS232_ENABLE ? icl3221_turn_on() : icl3221_turn_off();

        SYSTEM_FLAG &VOLT12_ENABLE ? power(1) : power(0);

        // Reset toggle flags
        state_machine_remove_flag(SYSTEM_ON);
        state_machine_remove_flag(DEPTH_ZERO);

        UCA0IE |= UCRXIE; // enable RX UART interrupt

        max3471_transmit('r');
        max3471_transmit('\n');
    }
}

void state_machine_init()
{
    TB0CTL |= TBCLR;        // reset TB0
    TB0CTL |= TBSSEL__ACLK; // ACLK
    TB0CTL |= MC__UP;       // up mode

    TB0CCR0 = TIMER_1S; // set the compare register

    TB0CCTL0 |= CCIE; // enable interrupt
    __enable_interrupt();
    TB0CCTL0 &= ~CCIFG; // clear interrupt
}

void state_machine_reset()
{
    SYSTEM_FLAG = 0;
    SYSTEM_LOOP_TIME_SECONDS = 1;
    SYSTEM_TIMER_INTERRUPT_COUNTER = 0;
}

//-------------------------------------------------------------------------------
// Interrupt Service Routine for TIMER0_B0 CCR0 register
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
    __bic_SR_register_on_exit(LPM1_bits);

    check_system_loop_time();
    TBCCTL0 &= ~CCIFG; // clear interrupt
}
