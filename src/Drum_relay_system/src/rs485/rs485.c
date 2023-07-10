#include "uart.h"
#include "rs485.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const struct device *const gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
#define GPIO_RS485_TX_ENABLE 5

static struct k_work work_uart_write;
static struct k_work_delayable work_uart_write_delayed;

#define MSG_SIZE 16
K_MSGQ_DEFINE(msgq_write, MSG_SIZE, 20, 4);
K_MSGQ_DEFINE(msgq_write_delayed, MSG_SIZE, 20, 4);

static void work_rs485_write(struct k_work *work)
{
    char *message;

    gpio_pin_set(gpio_dev, GPIO_RS485_TX_ENABLE, 1);
    k_usleep(1);

    k_msgq_get(&msgq_write, &message, K_NO_WAIT);

    uart_write(message);

    k_usleep(25);
    gpio_pin_set(gpio_dev, GPIO_RS485_TX_ENABLE, 0);
}

static void work_rs485_write_delayed(struct k_work *work)
{
    char *message;

    k_msgq_get(&msgq_write_delayed, &message, K_NO_WAIT);

    uart_write(message);
}

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
    rs485_tx_enable();
    uart_write(message);
    rs485_tx_disable();
    /*
    // Underwater sensor system is sampling and has disabled uart rx interrupt.
    // Takes approx 70ms to finish.
    if (uart_get_is_busy())
    {
        k_msgq_put(&msgq_write_delayed, &message, K_NO_WAIT);
        k_work_schedule(&work_uart_write_delayed, K_MSEC(500));
    }
    else
    {
        k_msgq_put(&msgq_write, &message, K_NO_WAIT);
        k_work_submit(&work_uart_write);
    }*/
}

void rs485_init()
{
    if (!device_is_ready(gpio_dev))
    {
        printk("GPIO device not found!");
        return 0;
    }

    gpio_pin_configure(gpio_dev, GPIO_RS485_TX_ENABLE, GPIO_OUTPUT);
    gpio_pin_set(gpio_dev, GPIO_RS485_TX_ENABLE, 0);

    k_work_init(&work_uart_write, work_rs485_write);
    k_work_init_delayable(&work_uart_write_delayed, work_rs485_write_delayed);

    uart_init();
}