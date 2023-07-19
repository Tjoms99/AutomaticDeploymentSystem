/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/adc.h>
#include <stdio.h>
#include <stdlib.h>

#include "gpio/leds.h"
#include "bluetooth/bluetooth.h"
#include "rs485/rs485.h"
#include "gpio/battery.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

int main(void)
{
	int ret = 0;

	// Show init status 1
	leds_init();
	rs485_init();
	bluetooth_init();
	leds_set_blue(1);

	battery_init();
	battery_charge_stop();
	// Led green on successful initialization.

	int battery_volt = 0;
	int length = 4;
	char battery_volt_s[length];

	while (1)
	{
		k_msleep(SLEEP_TIME_MS);
		battery_volt = battery_get_voltage();
		sprintf(battery_volt_s, "%d", battery_volt);
		bluetooth_write_data(DATA_BATTERY, &battery_volt_s, length);
	}

	return 0;
}