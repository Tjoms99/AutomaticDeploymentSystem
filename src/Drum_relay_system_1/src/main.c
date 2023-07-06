/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#include "gpio/leds.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

int main(void)
{
	// Show init status 1
	leds_init();

	// Led green on successful initialization.

	while (1)
	{
		leds_toggle_red();

		k_msleep(SLEEP_TIME_MS);
		leds_toggle_green();
		k_msleep(SLEEP_TIME_MS);
		leds_toggle_blue();
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}