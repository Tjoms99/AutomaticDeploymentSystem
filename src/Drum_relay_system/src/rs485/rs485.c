#include "uart.h"
#include "rs485.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const struct device *const gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
#define GPIO_RS485_TX_ENABLE 5

void rs485_tx_enable()
{
    gpio_pin_set(gpio_dev, GPIO_RS485_TX_ENABLE, 1);
    k_usleep(1);
}

void rs485_tx_disable()
{
    k_msleep(1);
    gpio_pin_set(gpio_dev, GPIO_RS485_TX_ENABLE, 0);
}

void rs485_write(char *message)
{
    while (uart_get_is_busy())
    {
        printk("UART busy");
        k_msleep(30);
    }

    rs485_tx_enable();
    uart_write(message);
    rs485_tx_disable();
}

void rs485_init()
{
    if (!device_is_ready(gpio_dev))
    {
        printk("GPIO device not found!");
        return;
    }

    gpio_pin_configure(gpio_dev, GPIO_RS485_TX_ENABLE, GPIO_OUTPUT);
    gpio_pin_set(gpio_dev, GPIO_RS485_TX_ENABLE, 0);

    uart_init();
}