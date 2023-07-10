/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "gpio/leds.h"
#include "bluetooth/bluetooth.h"
#include "rs485/rs485.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 5000

int main(void)
{
	// Show init status 1
	leds_init();
	rs485_init();
	bluetooth_init();
	leds_set_blue(1);
	// Led green on successful initialization.

	while (1)
	{
		k_msleep(SLEEP_TIME_MS);

		// rs485_write("4");

		// uart_print_depth();
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