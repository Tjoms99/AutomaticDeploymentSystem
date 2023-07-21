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

struct k_work battery_work;

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

void battery_work_handler(struct k_work *work_tem)
{
	float battery_volt = 0;
	int battery_percentage = 0;
	int length = 4;
	char battery_volt_s[length];

	battery_get_voltage(&battery_volt);
	battery_get_percentage(&battery_percentage, battery_volt);

	sprintf(battery_volt_s, "%d", battery_percentage);
	bluetooth_write_data(DATA_BATTERY, battery_volt_s, length);
}

int main(void)
{
	int ret = 0;

	ret |= leds_init();
	ret |= rs485_init();
	ret |= bluetooth_init();
	ret |= leds_set_blue(1);

	ret |= battery_init();
	ret |= battery_charge_start();

	if (ret)
	{
		printk("Initialization failed");
	}

	k_work_init(&battery_work, battery_work_handler);

	while (1)
	{
		k_msleep(SLEEP_TIME_MS);
		k_work_submit(&battery_work);
	}

	return 0;
}