#include "gpio/leds.h"
#include "bluetooth/bluetooth.h"
#include "rs485/rs485.h"
#include "gpio/battery.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define SLEEP_TIME_MS 1000

int main(void)
{
	int ret = 0;
	k_msleep(SLEEP_TIME_MS); // Gives time for the terminal to connect to catch LOG's

	ret |= leds_init();
	ret |= battery_init();
	ret |= rs485_init();
	ret |= bluetooth_init();
	ret |= leds_set_blue(1);

	ret |= battery_charge_start();

	if (ret)
	{
		LOG_ERR("Failed to initialize");
	}
	else
	{
		LOG_INF("Initialized");
	}

	while (1)
	{
		k_msleep(SLEEP_TIME_MS);
	}

	return 0;
}