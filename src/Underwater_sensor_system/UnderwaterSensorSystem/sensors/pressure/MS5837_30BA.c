/*
 * pressure.constants
 *
 *  Created on: 7 Dec 2022
 *      Author: marcu
 */

#include <MSP430.h>
#include <sensors/pressure/MS5837_30BA.h>
#include <i2c/i2c.h>

#define TIMER_33MS 10813 // 33ms

#define MS5837_30BA_ADDRESS 0x76

#define MS5837_30BA_RESET_SENSOR 0x1E
#define MS5837_30BA_GET_ADC_VALUE 0x00
#define MS5837_30BA_PROM_READ 0xA0
#define MS5837_30BA_START_CONVERTION_D1_4096 0x48
#define MS5837_30BA_START_CONVERTION_D2_4096 0x58

#define BYTES_2 0X02
#define BYTES_3 0X03

#define SEAWATER_DENSITY 1029   // kg/m^3
#define FRESHWATER_DENSITY 1000 // kg/m^3

static uint16_t coefficients[8];

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
// Cyclic redundancy check
static uint8_t crc4(uint16_t n_prom[])
{
    uint16_t n_rem = 0;
    uint8_t n_bit = 0;
    uint32_t i = 0;

    n_prom[0] = ((n_prom[0]) & 0x0FFF);
    n_prom[7] = 0;

    for (i = 0; i < 16; i++)
    {
        if (i & 0x01 == 1)
        {
            n_rem ^= (uint16_t)((n_prom[i >> 1]) & 0x00FF);
        }
        else
        {
            n_rem ^= (uint16_t)(n_prom[i >> 1] >> 8);
        }
        for (n_bit = 8; n_bit > 0; n_bit--)
        {
            if (n_rem & 0x8000)
            {
                n_rem = (n_rem << 1) ^ 0x3000;
            }
            else
            {
                n_rem = (n_rem << 1);
            }
        }
    }

    n_rem = ((n_rem >> 12) & 0x000F);

    return n_rem ^ 0x00;
}

void ms5847_30ba_calculate(float *pressure, float *temperature, uint32_t D1_pres, uint32_t D2_temp)
{
    static int32_t dT = 0;
    static int64_t SENS = 0;
    static int64_t OFF = 0;
    static int32_t SENSi = 0;
    static int32_t OFFi = 0;
    static int32_t Ti = 0;
    static int64_t OFF2 = 0;
    static int64_t SENS2 = 0;

    volatile int32_t TEMP;
    volatile int32_t P;

    // Temperature conversion first order
    dT = D2_temp - (uint32_t)coefficients[5] * 256l;
    TEMP = 2000l + coefficients[6] * (int64_t)dT / 8388608LL;

    // Pressure conversion first order
    SENS = (int64_t)coefficients[1] * 32768l + (int64_t)coefficients[3] * dT / 256l;
    OFF = (int64_t)coefficients[2] * 65536l + (int64_t)coefficients[4] * dT / 128l;
    P = (D1_pres * SENS / 2097152l - OFF) / (8192l);

    // Second order pressure and temperature calculations
    if ((TEMP / 100) >= 20) // High temp
    {
        Ti = 2 * ((int64_t)dT * (int64_t)dT) / (137438953472LL);
        OFFi = (1 * (TEMP - 2000) * (TEMP - 2000)) / 16;
        SENSi = 0;
    }

    if ((TEMP / 100) < 20) // Low temp
    {
        Ti = (3 * (int64_t)dT * (int64_t)dT) / 8589934592L;
        OFFi = (3 * (TEMP - 2000) * (TEMP - 2000)) / 2;
        SENSi = (5 * (TEMP - 2000) * (TEMP - 2000)) / 8;
    }

    if ((TEMP / 100) < -15) // Very low temp
    {
        OFFi = OFFi + 7 * (TEMP + 1500l) * (TEMP + 1500l);
        SENSi = SENSi + 4 * (TEMP + 1500l) * (TEMP + 1500l);
    }

    OFF2 = OFF - OFFi;
    SENS2 = SENS - SENSi;

    // Pressure & temperature
    P = (D1_pres * SENS2 / 2097152l - OFF2) / 8192l;
    TEMP = TEMP - Ti;

    *pressure = P;                //*10.0f;         //Pa
    *temperature = TEMP / 100.0f; // Celsius
}

// Wait for 33 seconds
// Set up timer compare register CCR1 and enter LPM
static void tsys01_wait_for_conversion()
{
    TB0CCTL2 |= CCIE; // enable interrupt

    // set register to trigger 33ms into the future
    TB0CCR2 = TB0R + TIMER_33MS; // set compare register
    if (TB0R + TIMER_33MS > TB0CCR0)
        TB0CCR2 = TIMER_33MS - (TB0CCR0 - TB0R); // in case of overflow when setting register

    __bis_SR_register(LPM1_bits | GIE);
}

void ms5847_30ba_init()
{
    uint32_t i = 0;

    i2c_write(MS5837_30BA_RESET_SENSOR, MS5837_30BA_ADDRESS); // reset sensor
    for (i = 0; i < 10000; i++)
        ;

    for (i = 0; i < 7; i++)
    {
        i2c_write((MS5837_30BA_PROM_READ + i * 2), MS5837_30BA_ADDRESS); // request coefficients
        i2c_read(BYTES_2, MS5837_30BA_ADDRESS);                          // read coefficients
        coefficients[i] = i2c_data_in;                                   // set coefficients
    }

    // Verify that data is correct with CRC
    uint8_t crcRead = coefficients[0] >> 12;
    uint8_t crcCalculated = crc4(coefficients);
    if (crcCalculated != crcRead)
        i = 69;

    timer_init();
}

void ms5847_30ba_start_pressure_convertion()
{
    i2c_write(MS5837_30BA_START_CONVERTION_D1_4096, MS5837_30BA_ADDRESS); // start conversion of pressure
}

void ms5847_30ba_start_temperature_convertion()
{
    i2c_write(MS5837_30BA_START_CONVERTION_D2_4096, MS5837_30BA_ADDRESS); // start conversion of temperature
}

void ms5847_30ba_get_convertion(uint32_t *adc_value)
{
    i2c_write(MS5837_30BA_GET_ADC_VALUE, MS5837_30BA_ADDRESS); // request ADC value
    i2c_read(BYTES_3, MS5837_30BA_ADDRESS);                    // read ADC value
    *adc_value = i2c_data_in;                                  // set ADC value
}

void get_conversion_values(uint32_t *D1, uint32_t *D2)
{
    ms5847_30ba_start_pressure_convertion();
    tsys01_wait_for_conversion();
    ms5847_30ba_get_convertion(D1);

    ms5847_30ba_start_temperature_convertion();
    tsys01_wait_for_conversion();
    ms5847_30ba_get_convertion(D2);
}

void ms5847_30ba_measure(float *pressure, float *temperature)
{
    static uint32_t D1_pres, D2_temp;
    get_conversion_values(&D1_pres, &D2_temp); // get ADC temperature and pressure values

    ms5847_30ba_calculate(pressure, temperature, D1_pres, D2_temp); // calculate pressure (Pa) and temperature (C)
}

// Calculate relative depth using a pressure reference
// Pressure reference should be taken just above the water level
void ms5847_30ba_get_depth(float *depth, float pressure, float pressure_reference)
{
    *depth = (pressure - pressure_reference) * 10.0f / (FRESHWATER_DENSITY * 9.80665); // h = P/(R*g)
}

// #pragma vector = TIMER0_B1_VECTOR
__interrupt void Timer_CCR2_ISR(void)
{
    switch (TB0IV)
    {
    case 0x04:
        __bic_SR_register_on_exit(LPM1_bits);
        break;
    default:
        break;
    }
}
