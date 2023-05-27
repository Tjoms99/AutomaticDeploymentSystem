
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
volatile uint8_t rs485_rx_data = 0;
volatile uint8_t rs232_rx_data = 0;

volatile uint16_t SYSTEM_LOOP_TIME_SECONDS = 1;
volatile uint16_t SYSTEM_TIMER_INTERRUPT_COUNTER = 0;

#define SYSTEM_ON BIT7
#define PRINT_ALL_REGISTERS BIT6
#define PRINT_CURRENT_REGISTER BIT5
#define CONTINUOUS_MODE BIT4
#define SINGLE_MODE BIT3
#define CUSTOM_TIME BIT2
#define RS232_ENABLE BIT1
#define POWER_ENABLE BIT0

static void check_system_loop_time()
{
    if (++SYSTEM_TIMER_INTERRUPT_COUNTER >= SYSTEM_LOOP_TIME_SECONDS)
    {
        SYSTEM_FLAG |= SYSTEM_ON;
        SYSTEM_TIMER_INTERRUPT_COUNTER = 0;
    }
}

static void set_system_loop_time(uint16_t seconds)
{
    SYSTEM_LOOP_TIME_SECONDS = seconds;
    SYSTEM_FLAG &= ~CUSTOM_TIME;
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
        SYSTEM_FLAG ^= POWER_ENABLE;
    if (data == '1')
        SYSTEM_FLAG ^= RS232_ENABLE;
    if (data == '2')
        SYSTEM_FLAG ^= CUSTOM_TIME;
    if (data == '3')
        SYSTEM_FLAG ^= SINGLE_MODE;
    if (data == '4')
        SYSTEM_FLAG ^= CONTINUOUS_MODE;
    if (data == '5')
        SYSTEM_FLAG ^= PRINT_CURRENT_REGISTER;
    if (data == '6')
        SYSTEM_FLAG ^= PRINT_ALL_REGISTERS;
    if (data == '7')
        SYSTEM_FLAG = 0; // Reset
}

void print_current_temperature()
{
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

void print_temperature_register()
{
    float temperature = 0.0;
    char buffer[6];
    uint8_t done_reading = 0x00;

    do
    {
        get_temperature_next_register(&temperature, &done_reading);

        float_to_char_array(temperature, buffer);
        max3471_transmit(12);
        max3471_transmit(buffer[0]);
        max3471_transmit(buffer[1]);
        max3471_transmit(buffer[2]);
        max3471_transmit(buffer[3]);
        max3471_transmit(buffer[4]);
        max3471_transmit(buffer[5]);
    } while (!done_reading);

    SYSTEM_FLAG ^= PRINT_ALL_REGISTERS;
}

// Sample pressure and temperature once
void single_mode()
{
    static float TSYS01_temperature = 0;

    TSYS01_measure(&TSYS01_temperature);
    set_temperature_current_register(TSYS01_temperature);

    SYSTEM_FLAG ^= SINGLE_MODE;
}

// Sample pressure and temperature once
void continuous_mode()
{
    static float TSYS01_temperature = 0;

    TSYS01_measure(&TSYS01_temperature);
    set_temperature_current_register(TSYS01_temperature);
    set_temperature_next_register(TSYS01_temperature);
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

    // RS485 Rx does not work when LPMx > 1
    // Reason: DC0 takes to long to start up / drifts
    while (1)
    {

        if (SYSTEM_FLAG & SYSTEM_ON)
        {

            SYSTEM_FLAG &POWER_ENABLE ? power(0xFF) : power(0x00);

            SYSTEM_FLAG &SINGLE_MODE ? single_mode() : __no_operation;

            SYSTEM_FLAG &CONTINUOUS_MODE ? continuous_mode() : __no_operation;

            SYSTEM_FLAG &PRINT_CURRENT_REGISTER ? print_current_temperature() : __no_operation;

            SYSTEM_FLAG &PRINT_ALL_REGISTERS ? print_temperature_register() : __no_operation;

            SYSTEM_FLAG &= ~SYSTEM_ON;
        }

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
    SYSTEM_FLAG &CUSTOM_TIME ? set_system_loop_time(rs485_rx_data / 10) : update_system_flags(rs485_rx_data);
}

// Interrupt Service Routine for UART receive
#pragma vector = USCI_A1_VECTOR
__interrupt void RS232_ISR(void)
{
    __bic_SR_register_on_exit(LPM1_bits);

    rs232_rx_data = UCA1RXBUF;
    update_system_flags(rs232_rx_data);
}