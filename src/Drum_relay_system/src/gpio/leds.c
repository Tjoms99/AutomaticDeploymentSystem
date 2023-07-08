#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include "leds.h"

static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

void leds_init()
{
    int ret;

    if (!gpio_is_ready_dt(&led_red) && !gpio_is_ready_dt(&led_green) && !gpio_is_ready_dt(&led_blue))
    {
        return 0;
    }

    ret = gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_ACTIVE);
    ret = gpio_pin_configure_dt(&led_green, GPIO_OUTPUT_ACTIVE);
    ret = gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_ACTIVE);

    ret = gpio_pin_set_dt(&led_red, 0);
    ret = gpio_pin_set_dt(&led_green, 0);
    ret = gpio_pin_set_dt(&led_blue, 0);

    if (ret < 0)
    {
        return 0;
    }
}

void leds_set_red(uint8_t pin_state)
{
    gpio_pin_set_dt(&led_red, pin_state);
}

void leds_set_green(uint8_t pin_state)
{
    gpio_pin_set_dt(&led_green, pin_state);
}

void leds_set_blue(uint8_t pin_state)
{
    gpio_pin_set_dt(&led_blue, pin_state);
}

void leds_toggle_red()
{
    gpio_pin_toggle_dt(&led_red);
}

void leds_toggle_green()
{
    gpio_pin_toggle_dt(&led_green);
}

void leds_toggle_blue()
{
    gpio_pin_toggle_dt(&led_blue);
}