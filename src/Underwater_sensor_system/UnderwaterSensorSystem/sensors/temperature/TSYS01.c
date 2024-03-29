/*
 * temp.c
 *
 *  Created on: 5 Dec 2022
 *      Author: Marcus Alexander Tjomsaas
 */

#include <stdint.h>
#include <MSP430.h>
#include <sensors/temperature/TSYS01.h>
#include <i2c/i2c.h>

#define TIMER_33MS 10813 // 33ms

static uint8_t is_finished_converting = 0;

static void timer_init()
{

    TB0CTL |= TBCLR;        // reset TB0
    TB0CTL |= TBSSEL__ACLK; // ACLK
    TB0CTL |= MC__UP;       // up mode

    TB0CCR1 = TIMER_33MS; // set the compare register
    TB0CCTL1 &= ~CAP;

    TB0CCTL1 |= CCIE; // enable interrupt
    __enable_interrupt();
    TB0CCTL1 &= ~CCIFG; // clear interrupt
}
static void set_coefficients(uint8_t coefficient)
{
    if (coefficient == K4)
        k4 = i2c_data_in;
    if (coefficient == K3)
        k3 = i2c_data_in;
    if (coefficient == K2)
        k2 = i2c_data_in;
    if (coefficient == K1)
        k1 = i2c_data_in;
    if (coefficient == K0)
        k0 = i2c_data_in;
}

// Wait for 33 seconds
// Set up timer compare register CCR1 and enter LPM
void tsys01_wait_for_conversion()
{
    TBCCTL1 |= CCIE; // enable interrupt

    // set register to trigger 33ms into the future
    TB0CCR1 = TB0R + TIMER_33MS; // set compare register
    if (TB0R + TIMER_33MS > TB0CCR0)
        TB0CCR1 = TIMER_33MS - (TB0CCR0 - TB0R); // in case of overflow when setting register

    __bis_SR_register(LPM1_bits | GIE);
    while(!is_finished_converting);

    is_finished_converting = 0;
    TB0CCTL1 |= CCIE; // disable interrupt
}

void tsys01_start_convertion()
{
    i2c_write(TSYS01_START_CONVERSION, TSYS01_ADDRESS); // start conversion
}

void tsys01_get_convertion(float *adc_value)
{
    i2c_write(TSYS01_GET_ADC_VALUE, TSYS01_ADDRESS); // read adc temp value
    i2c_read(BYTES_2, TSYS01_ADDRESS);
    *adc_value = i2c_data_in;
}

void tsys01_calculate_temperature(float *temperature, float adc_value)
{
    float adc = adc_value; // Default value 9378708 / 256; (for CRC)

    float k4_t = -1 * (2.0f * k4 / 1000000000000000000000.0f * adc * adc * adc * adc);
    float k3_t = (4.0f * k3 / 10000000000000000.0f * adc * adc * adc);
    float k2_t = -1 * (2.0f * k2 / 100000000000.0f * adc * adc);
    float k1_t = (1.0f * k1 / 1000000.0f * adc);
    float k0_t = -1 * (1.5f * k0 / 100.0f);

    *temperature = k4_t + k3_t + k2_t + k1_t + k0_t;
}

void tsys01_init()
{
    uint32_t index;

    i2c_write(TSYS01_RESET_SENSOR, TSYS01_ADDRESS); // reset sensor
    for (index = 0; index < 10000; index++)
        ;

    i2c_write(TSYS01_GET_K4, TSYS01_ADDRESS); // get k4
    i2c_read(BYTES_2, TSYS01_ADDRESS);
    set_coefficients(K4);

    i2c_write(TSYS01_GET_K3, TSYS01_ADDRESS); // get k3
    i2c_read(BYTES_2, TSYS01_ADDRESS);
    set_coefficients(K3);

    i2c_write(TSYS01_GET_K2, TSYS01_ADDRESS); // get k2
    i2c_read(BYTES_2, TSYS01_ADDRESS);
    set_coefficients(K2);

    i2c_write(TSYS01_GET_K1, TSYS01_ADDRESS); // get k1
    i2c_read(BYTES_2, TSYS01_ADDRESS);
    set_coefficients(K1);

    i2c_write(TSYS01_GET_K0, TSYS01_ADDRESS); // get k0
    i2c_read(BYTES_2, TSYS01_ADDRESS);
    set_coefficients(K0);

    timer_init();
}

void tsys01_measure(float *temperature)
{
    float tsys01_adc = 0;

    tsys01_start_convertion();
    tsys01_wait_for_conversion();
    tsys01_get_convertion(&tsys01_adc);
    tsys01_calculate_temperature(temperature, tsys01_adc);
}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void Timer_CCR1_ISR(void)
{
    switch (TB0IV)
    {
    case 0x02:
        __bic_SR_register_on_exit(LPM1_bits);
        is_finished_converting = 1;
        break;
    default:
        break;
    }
}
