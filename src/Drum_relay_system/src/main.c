/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/adc.h>

#include "gpio/leds.h"
#include "bluetooth/bluetooth.h"
#include "rs485/rs485.h"
#include "gpio/battery.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

static const struct device *adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc));

#define ADC_RESOLUTION 10
#define ADC_CHANNEL 7
#define ADC_PORT SAADC_CH_PSELP_PSELP_AnalogInput0 // AIN7
#define ADC_REFERENCE ADC_REF_INTERNAL			   // 0.6V
#define ADC_GAIN ADC_GAIN_1_5					   // ADC REFERENCE * 5 // ADC_GAIN_1_6

struct adc_channel_cfg channel_7_cfg = {
	.gain = ADC_GAIN,
	.reference = ADC_REFERENCE,
	.acquisition_time = ADC_ACQ_TIME_DEFAULT,
	.channel_id = ADC_CHANNEL,
#ifdef CONFIG_ADC_NRFX_SAADC
	.input_positive = ADC_PORT
#endif
};

int16_t sample_buffer[1];
struct adc_sequence sequence = {
	.channels = BIT(ADC_CHANNEL),
	.buffer = sample_buffer,
	.buffer_size = sizeof(sample_buffer),
	.resolution = ADC_RESOLUTION};

int main(void)
{
	int ret = 0;

	// Show init status 1
	leds_init();
	rs485_init();
	bluetooth_init();
	leds_set_blue(1);

	battery_init();
	battery_charge_start();
	// Led green on successful initialization.
	// ADC
	if (!device_is_ready(adc_dev))
	{
		printk("ADC device not found!");
		return -EIO;
	}

	ret |= adc_channel_setup(adc_dev, &channel_7_cfg);

	if (ret)
	{
		printk("ADC setup failed (error %d)", ret);
	}

	while (1)
	{
		k_msleep(SLEEP_TIME_MS);

		ret |= adc_read(adc_dev, &sequence);
		if (ret)
		{
			printk("ADC read failed (error %d)", ret);
		}

		int32_t mv_value = sample_buffer[0];
		printk("ADC value: %d \n", mv_value);

		int32_t adc_vref = adc_ref_internal(adc_dev);
		ret |= adc_raw_to_millivolts(adc_vref, ADC_GAIN, ADC_RESOLUTION, &mv_value);
		printk("ADC volt: %d mv\n", mv_value);
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