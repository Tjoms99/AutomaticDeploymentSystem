/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

// #include "gpio/leds.h"
#include "bluetooth/bluetooth.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

int main(void)
{
	// Show init status 1
	// leds_init();
	bluetooth_init();
	// Led green on successful initialization.

	while (1)
	{
		bluetooth_write_data(DATA_DEPTH, "-1.321", 6);
		bluetooth_write_data(DATA_PRESSURE, "103123", 6);
		bluetooth_write_data(DATA_TEMPERATURE, "24.123", 6);
		k_msleep(SLEEP_TIME_MS);

		/*
		leds_toggle_red();

		leds_toggle_green();
		k_msleep(SLEEP_TIME_MS);
		leds_toggle_blue();
		k_msleep(SLEEP_TIME_MS);
	*/
	}
	return 0;
}