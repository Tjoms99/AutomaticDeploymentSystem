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

    if (ret < 0)
    {
        return 0;
    }
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