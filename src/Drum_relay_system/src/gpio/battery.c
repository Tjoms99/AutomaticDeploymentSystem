#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "battery.h"

static const struct device *const gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));

#define GPIO_BATTERY_CHARGE_SPEED 13
#define GPIO_BATTERY_CHARGING_ENABLE 17

#define VOLT_PER_STEP (0.003515625F) // 3.6 reference and 10 bit resolution

void battery_fast_charge()
{
    gpio_pin_set(gpio_dev, GPIO_BATTERY_CHARGE_SPEED, 1); // FAST charge 100mA
}

void battery_slow_charge()
{
    gpio_pin_set(gpio_dev, GPIO_BATTERY_CHARGE_SPEED, 0); // SLOW charge 50mA
}

void battery_charge_start()
{
    gpio_pin_set(gpio_dev, GPIO_BATTERY_CHARGING_ENABLE, 1);
}

void battery_charge_stop()
{
    gpio_pin_set(gpio_dev, GPIO_BATTERY_CHARGING_ENABLE, 0);
}

float battery_get_voltage()
{
    battery_charge_stop();
    // TODO: Read actual adc value
    uint32_t adc_value = 750; // 10 bit
    float adc_voltage = adc_value * VOLT_PER_STEP;
    float battery_voltage = adc_voltage * (1000.0 / 510.0);

    return battery_voltage;
}

float battery_get_percentage()
{
    const float volt_reference_top = 4.2;
    const float volt_reference_bottom = 3.6;
    float volt_reference_difference = volt_reference_top - volt_reference_bottom;

    float volt = battery_get_voltage();
    float volt_difference = volt - volt_reference_bottom;
    float volt_percent = volt_difference / volt_reference_difference * 100.0;

    return volt_percent;
}

void battery_init()
{
    if (!device_is_ready(gpio_dev))
    {
        printk("GPIO device not found!");
        return;
    }

    gpio_pin_configure(gpio_dev, GPIO_BATTERY_CHARGING_ENABLE, GPIO_OUTPUT);

    gpio_pin_configure(gpio_dev, GPIO_BATTERY_CHARGE_SPEED, GPIO_OUTPUT | GPIO_ACTIVE_LOW);
    battery_fast_charge();
}
/* TODO: STOLEN FROM SOMEWHERE, INTEGRATE!!
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