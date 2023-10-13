
#include <MSP430.h>

#define WatchDog (WDTPW | WDTCNTCL | WDTSSEL__ACLK | WDTIS_4) // 1 s (depends on clock configuration)

/*
 *  Watchdog timer interval select (WDTIS_x). These bits select the watchdog timer interval to  set the WDTIFG flag or generate a PUC.
 *  000b = Watchdog clock source / 2^31 (18:12:16 at 32.768 kHz)
 *  001b = Watchdog clock source / 2^27 (01:08:16 at 32.768 kHz)
 *  010b = Watchdog clock source / 2^23 (00:04:16 at 32.768 kHz)
 *  011b = Watchdog clock source / 2^19 (00:00:16 at 32.768 kHz)
 *  100b = Watchdog clock source / 2^15 (1 s at 32.768 kHz)
 *  101b = Watchdog clock source / 2^13 (250 ms at 32.768 kHz)
 *  110b = Watchdog clock source / 2^9  (15.625 ms at 32.768 kHz)
 *  111b = Watchdog clock source / 2^6  (1.95 ms at 32.768 kHz)
 */

void watchdog_hold(void)
{
    // Watchdog password write + halt.
    WDTCTL = WDTPW | WDTHOLD;
}
void watchdog_kick(void)
{
    // Watchdog password write + register clear + set clock to ACLK + interval 4 (1 second).

    WDTCTL = WDTPW | WDTCNTCL | WDTSSEL__ACLK | WDTIS_4;
}
