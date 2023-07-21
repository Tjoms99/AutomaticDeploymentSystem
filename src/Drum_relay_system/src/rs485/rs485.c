#include "uart.h"
#include "rs485.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(rs485, LOG_LEVEL_INF);

static const struct device *const gpio_rs485_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
#define GPIO_RS485_TX_ENABLE 5

static void rs485_tx_enable()
{
    gpio_pin_set(gpio_rs485_dev, GPIO_RS485_TX_ENABLE, 1);
    k_usleep(1); // Delay to assure the pin transition from low to high.
}

static void rs485_tx_disable()
{
    k_msleep(1); // Needed due to the UART write poll register needs time to transmitt data.
    gpio_pin_set(gpio_rs485_dev, GPIO_RS485_TX_ENABLE, 0);
}

void rs485_write(char *message)
{
    // Wait for the RS485 bus to become available.
    while (uart_get_is_busy())
    {
        LOG_DBG("UART busy");
        k_msleep(30);
    }

    rs485_tx_enable();
    uart_write(message);
    rs485_tx_disable();

    LOG_DBG("Message: %s", message);
}

int rs485_init()
{
    int ret = 0;

    if (!device_is_ready(gpio_rs485_dev))
    {
        LOG_ERR("GPIO device not found!");
        return -EIO;
    }

    ret |= gpio_pin_configure(gpio_rs485_dev, GPIO_RS485_TX_ENABLE, GPIO_OUTPUT);
    ret |= gpio_pin_set(gpio_rs485_dev, GPIO_RS485_TX_ENABLE, 0);

    ret |= uart_init();

    if (ret)
    {
        LOG_ERR("Initialization failed (error %d)", ret);
        return ret;
    }

    LOG_INF("Initialized");
    return ret;
}