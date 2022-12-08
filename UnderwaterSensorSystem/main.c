
#include <MSP430.h>
#include <stdint.h>
#include "i2c/i2c.h"
#include "temperature/temp.h"

uint32_t index;

//Note for next time:
//  Add pressure driver
//  Add timer interval of 1 sec to sample temperature and pressure
int main(void)
{

    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    init_i2c();
    for(index = 0; index <10000; index++);
    init_temp();

    while(1)
    {
        volatile static double temperature = 1;

        temperature = get_temperature();

        for(index = 0; index <10000; index++);

    }
}
