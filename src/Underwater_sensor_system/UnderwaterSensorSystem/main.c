
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
#include <state_machine/state_machine.h>
#include <stdint.h>

volatile uint8_t rs485_rx_data = 0;
volatile uint8_t rs232_rx_data = 0;

int main(void)
{
    //---------------------------------
    // System clock
    watchdog_hold();
    clock_init_16mhz();

    //---------------------------------
    // Start state machine timer
    state_machine_init();

    //---------------------------------
    // Communication
    i2c_init();

    max3471_init();
    max3471_set_mode(0);

    icl3221_init();
    icl3221_set_mode(1);

    //---------------------------------
    // External components
    power_init();
    sensors_init();

    //---------------------------------
    // Initialize values
    sensors_sample();
    sensors_set_depth_zero();

    while (1)
    {
        watchdog_kick();
        state_machine_loop();
        watchdog_kick();

        // Enter low-power mode
        // RS485 RX interrupt does not work when LPMx > 1
        // Reason: DC0 takes to long to start up / drifts
        __bis_SR_register(LPM1_bits | GIE);
    }
}

//-------------------------------------------------------------------------------

// Interrupt Service Routine for UART receive
#pragma vector = USCI_A0_VECTOR
__interrupt void RS485_ISR(void)
{
    __bic_SR_register_on_exit(LPM1_bits);

    rs485_rx_data = UCA0RXBUF;

    state_machine_update_flag(rs485_rx_data);
}

// Interrupt Service Routine for UART receive
#pragma vector = USCI_A1_VECTOR
__interrupt void RS232_ISR(void)
{
    __bic_SR_register_on_exit(LPM1_bits);

    rs232_rx_data = UCA1RXBUF;
    state_machine_update_flag(rs232_rx_data);
}
