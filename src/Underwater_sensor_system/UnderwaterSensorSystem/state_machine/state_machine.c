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
// Usefull to keep state even though the watchdog has kicked.
#pragma PERSISTENT(SYSTEM_FLAG)
uint8_t SYSTEM_FLAG = 0;

#pragma PERSISTENT(SYSTEM_FLAG)
uint16_t SYSTEM_LOOP_TIME_SECONDS = 1;

#pragma PERSISTENT(SYSTEM_FLAG)
uint16_t SYSTEM_TIMER_INTERRUPT_COUNTER = 0;

#define TIMER_1S 0x7FFF /// 7FFF 1s

/**
 * SYSTEM FLAGS
 *  0 - Power enable: 12V
 *  1 - RS232 enable
 *  2 - Set custom time interval used in continuous mode. First enable flag then send time interval
 *  3 - Single mode: One pressure + temperature sample
 *  4 - Continuous mode: Continuously sample pressure and temperature
 *  5 - Print current temperature
 *  6 - Print temperature registers
 *  7 - Reset: Set all flags to 0
 *
 */
void state_machine_update_flag(char data)
{
    if (data == '0')
        SYSTEM_FLAG ^= VOLT12_ENABLE;
    if (data == '1')
        SYSTEM_FLAG ^= RS232_ENABLE;
    if (data == '2')
        SYSTEM_FLAG ^= CUSTOM_TIME;
    if (data == '3')
        SYSTEM_FLAG ^= DEPTH_ZERO;
    if (data == '4')
        SYSTEM_FLAG ^= CONTINUOUS_MODE;
    if (data == '5')
        SYSTEM_FLAG ^= PRINT_CURRENT_REGISTER;
    if (data == '6')
        SYSTEM_FLAG ^= PRINT_ALL_REGISTERS;
    if (data == '7')
        state_machine_reset();
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

void state_machine_set_loop_interval(char seconds_c)
{
    static uint8_t number_of_chars_to_recieve = 3;
    static uint16_t seconds = 0;

    if (number_of_chars_to_recieve == 3)
    {
        uint16_t seconds_100 = (uint16_t)seconds_c - 48; // ASCII '0' character
        seconds = seconds_100 * 100;
    }
    else if (number_of_chars_to_recieve == 2)
    {
        uint16_t seconds_10 = (uint16_t)seconds_c - 48; // ASCII '0' character
        seconds += seconds_10 * 10;
    }
    else if (number_of_chars_to_recieve == 1)
    {
        uint16_t seconds_1 = (uint16_t)seconds_c - 48; // ASCII '0' character
        seconds += seconds_1;
    }

    number_of_chars_to_recieve--;

    if (number_of_chars_to_recieve == 0)
    {
        SYSTEM_LOOP_TIME_SECONDS = seconds;
        SYSTEM_FLAG &= ~CUSTOM_TIME;
        number_of_chars_to_recieve = 3;
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
        SYSTEM_FLAG &VOLT12_ENABLE ? power(1) : power(0);

        SYSTEM_FLAG &DEPTH_ZERO ? set_pressure_at_zero_depth() : __no_operation;

        SYSTEM_FLAG &CONTINUOUS_MODE ? sensors_sample_and_print() : __no_operation;

        SYSTEM_FLAG &PRINT_ALL_REGISTERS ? print_temperature_register() : __no_operation;

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

#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
    __bic_SR_register_on_exit(LPM1_bits);

    check_system_loop_time();
    TBCCTL0 &= ~CCIFG; // clear interrupt
}
