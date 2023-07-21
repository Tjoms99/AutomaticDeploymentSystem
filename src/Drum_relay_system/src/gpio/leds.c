#include "leds.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(leds, LOG_LEVEL_INF);

static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

static uint8_t is_initialized = false;

int leds_set_red(uint8_t pin_state)
{
    if (!is_initialized)
    {
        return -ECANCELED;
    }

    return gpio_pin_set_dt(&led_red, pin_state);
}

int leds_set_green(uint8_t pin_state)
{
    if (!is_initialized)
    {
        return -ECANCELED;
    }

    return gpio_pin_set_dt(&led_green, pin_state);
}

int leds_set_blue(uint8_t pin_state)
{
    if (!is_initialized)
    {
        return -ECANCELED;
    }

    return gpio_pin_set_dt(&led_blue, pin_state);
}

int leds_toggle_red()
{
    if (!is_initialized)
    {
        return -ECANCELED;
    }

    return gpio_pin_toggle_dt(&led_red);
}

int leds_toggle_green()
{
    if (!is_initialized)
    {
        return -ECANCELED;
    }

    return gpio_pin_toggle_dt(&led_green);
}

int leds_toggle_blue()
{
    if (!is_initialized)
    {
        return -ECANCELED;
    }

    return gpio_pin_toggle_dt(&led_blue);
}

int leds_init()
{
    int ret = 0;

    if (!gpio_is_ready_dt(&led_red) && !gpio_is_ready_dt(&led_green) && !gpio_is_ready_dt(&led_blue))
    {
        LOG_ERR("GPIO devices not found!");
        return -EIO;
    }

    ret |= gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_ACTIVE);
    ret |= gpio_pin_configure_dt(&led_green, GPIO_OUTPUT_ACTIVE);
    ret |= gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_ACTIVE);

    ret |= gpio_pin_set_dt(&led_red, 0);
    ret |= gpio_pin_set_dt(&led_green, 0);
    ret |= gpio_pin_set_dt(&led_blue, 0);

    if (ret)
    {
        LOG_ERR("Initialization failed (error %d)", ret);
        return ret;
    }
    LOG_INF("Initialized");

    is_initialized = true;

    return ret;
}