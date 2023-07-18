#include "battery.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>

static const struct device *const gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
static const struct device *adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc));

#define GPIO_BATTERY_CHARGE_SPEED 13
#define GPIO_BATTERY_CHARGING_ENABLE 17
#define GPIO_BATTERY_READ_ENABLE 14

static uint8_t is_initialized = false;

#define ADC_RESOLUTION 10
#define ADC_CHANNEL 7
#define ADC_PORT SAADC_CH_PSELP_PSELP_AnalogInput0 // AIN7
#define ADC_REFERENCE ADC_REF_INTERNAL             // 0.6V
#define ADC_GAIN ADC_GAIN_1_6                      // ADC REFERENCE * 5 // ADC_GAIN_1_6 ADC REFERENCE * 6

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

static int battery_enable_read()
{
    return gpio_pin_set(gpio_dev, GPIO_BATTERY_READ_ENABLE, 0);
}
int battery_set_fast_charge()
{
    if (!is_initialized)
    {
        return -ECANCELED;
    }

    return gpio_pin_set(gpio_dev, GPIO_BATTERY_CHARGE_SPEED, 1); // FAST charge 100mA
}

int battery_set_slow_charge()
{
    if (!is_initialized)
    {
        return -ECANCELED;
    }

    return gpio_pin_set(gpio_dev, GPIO_BATTERY_CHARGE_SPEED, 0); // SLOW charge 50mA
}

int battery_charge_start()
{
    if (!is_initialized)
    {
        return -ECANCELED;
    }

    return gpio_pin_set(gpio_dev, GPIO_BATTERY_CHARGING_ENABLE, 1);
}

int battery_charge_stop()
{
    if (!is_initialized)
    {
        return -ECANCELED;
    }

    return gpio_pin_set(gpio_dev, GPIO_BATTERY_CHARGING_ENABLE, 0);
}

int battery_get_voltage()
{
    int ret = 0;

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

    // int battery_volt = mv_value *rc = val * (uint64_t)dcp->full_ohm / dcp->output_ohm;

    return mv_value;

    return 0;
}

int battery_get_percentage()
{

    return 0;
}

int battery_init()
{
    int ret = 0;

    // GPIO
    if (!device_is_ready(gpio_dev))
    {
        printk("GPIO device not found!");
        // return -EIO;
    }

    ret |= gpio_pin_configure(gpio_dev, GPIO_BATTERY_CHARGING_ENABLE, GPIO_OUTPUT);
    ret |= gpio_pin_configure(gpio_dev, GPIO_BATTERY_READ_ENABLE, GPIO_OUTPUT);
    ret |= gpio_pin_configure(gpio_dev, GPIO_BATTERY_CHARGE_SPEED, GPIO_OUTPUT | GPIO_ACTIVE_LOW);
    ret |= battery_enable_read();
    ret |= battery_set_fast_charge();
    if (ret)
    {
        printk("GPIO configure failed!");
        // return ret;
    }

    if (!device_is_ready(adc_dev))
    {
        printk("ADC device not found!");
        // return -EIO;
    }

    ret |= adc_channel_setup(adc_dev, &channel_7_cfg);

    if (ret)
    {
        printk("ADC setup failed (error %d)", ret);
    }
    is_initialized = true;

    return ret;
}

/* TODO: STOLEN FROM ARDUINO LIBRARY, INTEGRATE!!
Xiao::Xiao() {
  pinMode(VBAT_ENABLE, OUTPUT);
  pinMode(BAT_CHARGE_STATE, INPUT);

  digitalWrite(BAT_HIGH_CHARGE, HIGH); // charge with 100mA
}


float Xiao::GetBatteryVoltage() {
  digitalWrite(VBAT_ENABLE, LOW);



  digitalWrite(VBAT_ENABLE, HIGH);

  return vBat;
}

bool Xiao::IsChargingBattery() { return digitalRead(BAT_CHARGE_STATE) == LOW; }
*/