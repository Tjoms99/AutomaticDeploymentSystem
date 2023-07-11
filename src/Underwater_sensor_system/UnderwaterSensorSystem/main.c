
#include <MSP430.h>
#include <rs485/max3471.h>
#include <rs485/print.h>
#include <rs232/icl3221.h>
#include <clock/clock.h>
#include <clock/watchdog.h>
#include <power/power.h>

#include <stdint.h>

#include <sensors/pressure/MS5837_30BA.h>
#include <sensors/sensors.h>

#include <i2c/i2c.h>
#include <storage/memory.h>
#define TIMER_1S 0x7FFF /// 7FFF 1s

#pragma PERSISTENT(SYSTEM_FLAG)
uint8_t SYSTEM_FLAG = 0;
volatile uint8_t rs485_rx_data = 0;
volatile uint8_t rs232_rx_data = 0;

volatile uint16_t SYSTEM_LOOP_TIME_SECONDS = 1;
volatile uint16_t SYSTEM_TIMER_INTERRUPT_COUNTER = 0;

#define SYSTEM_ON BIT7
#define PRINT_ALL_REGISTERS BIT6
#define PRINT_CURRENT_REGISTER BIT5
#define CONTINUOUS_MODE BIT4
#define DEPTH_ZERO BIT3
#define CUSTOM_TIME BIT2
#define RS232_ENABLE BIT1
#define VOLT12_ENABLE BIT0

static void check_system_loop_time()
{
    if (++SYSTEM_TIMER_INTERRUPT_COUNTER >= SYSTEM_LOOP_TIME_SECONDS)
    {
        SYSTEM_FLAG |= SYSTEM_ON;
        SYSTEM_TIMER_INTERRUPT_COUNTER = 0;
    }
}

static void set_system_loop_time(char seconds_c)
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

static void timer_init()
{

    TB0CTL |= TBCLR;        // reset TB0
    TB0CTL |= TBSSEL__ACLK; // ACLK
    TB0CTL |= MC__UP;       // up mode

    TB0CCR0 = TIMER_1S; // set the compare register

    TB0CCTL0 |= CCIE; // enable interrupt
    __enable_interrupt();
    TB0CCTL0 &= ~CCIFG; // clear interrupt
}

void reset()
{
    SYSTEM_FLAG = 0;
    SYSTEM_LOOP_TIME_SECONDS = 1;
    SYSTEM_TIMER_INTERRUPT_COUNTER = 0;
}
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
void update_system_flags(char data)
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
        reset();
}

// Sample pressure and temperature once
void set_pressure_at_zero_depth()
{
    static float ms5847_30ba_pressure = 0;
    get_pressure_current_register(&ms5847_30ba_pressure);
    sensors_pressure_at_zero_depth(ms5847_30ba_pressure);
    SYSTEM_FLAG ^= DEPTH_ZERO;
}

// Sample pressure and temperature once
void continuous_mode()
{
    static float tsys01_temperature = 0;
    static float ms5847_30ba_pressure = 0;
    static float ms5847_30ba_depth = 0;

    sensors_get_values(&tsys01_temperature, &ms5847_30ba_pressure, &ms5847_30ba_depth);

    set_temperature_current_register(tsys01_temperature);
    set_temperature_next_register(tsys01_temperature);

    set_pressure_current_register(ms5847_30ba_pressure);
    set_pressure_next_register(ms5847_30ba_pressure);

    set_depth_current_register(ms5847_30ba_depth);
    set_depth_next_register(ms5847_30ba_depth);

    print_current_values();
}

int main(void)
{
    // Activated by default to trigger every 32ms, should be first.
    watchdog_hold();

    clock_init_16mhz();
    timer_init();
    i2c_init();

    max3471_init();
    max3471_set_mode(0);

    icl3221_init();
    icl3221_set_mode(1);
    icl3221_turn_on();

    power_init();
    sensors_init();

    continuous_mode();
    set_pressure_at_zero_depth();
    // RS485 Rx does not work when LPMx > 1
    // Reason: DC0 takes to long to start up / drifts
    while (1)
    {
        watchdog_kick();

        if (SYSTEM_FLAG & SYSTEM_ON)
        {
            max3471_transmit('b');
            max3471_transmit('\n');

            UCA0IE &= ~UCRXIE; // disable RX UART INT

            SYSTEM_FLAG &VOLT12_ENABLE ? power(1) : power(0);

            SYSTEM_FLAG &DEPTH_ZERO ? set_pressure_at_zero_depth() : __no_operation;

            SYSTEM_FLAG &CONTINUOUS_MODE ? continuous_mode() : __no_operation;

            SYSTEM_FLAG &PRINT_ALL_REGISTERS ? print_temperature_register() : __no_operation;

            SYSTEM_FLAG &= ~SYSTEM_ON;

            UCA0IE |= UCRXIE; // enable RX UART INT
            max3471_transmit('r');
            max3471_transmit('\n');
        }

        // Triggered again as SYSTEM_ON triggers exacly at the same time (every second) as the watchdog.
        watchdog_kick();
        // Enter low-power mode
        __bis_SR_register(LPM1_bits | GIE);
    }
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
    __bic_SR_register_on_exit(LPM1_bits);

    check_system_loop_time();
    TBCCTL0 &= ~CCIFG; // clear interrupt
}

// Interrupt Service Routine for UART receive
#pragma vector = USCI_A0_VECTOR
__interrupt void RS485_ISR(void)
{
    __bic_SR_register_on_exit(LPM1_bits);

    rs485_rx_data = UCA0RXBUF;

    // Remove / 10 when not using ASCII chars over putty
    SYSTEM_FLAG &CUSTOM_TIME ? set_system_loop_time(rs485_rx_data) : update_system_flags(rs485_rx_data);
}

// Interrupt Service Routine for UART receive
#pragma vector = USCI_A1_VECTOR
__interrupt void RS232_ISR(void)
{
    __bic_SR_register_on_exit(LPM1_bits);

    rs232_rx_data = UCA1RXBUF;
    update_system_flags(rs232_rx_data);
}
