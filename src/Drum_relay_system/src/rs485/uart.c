#include "uart.h"
#include "../bluetooth/bluetooth.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(uart, LOG_LEVEL_INF);

#include <string.h>

static const struct device *const uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));

#define MESSAGE_SIZE 16
static char rx_buf[MESSAGE_SIZE];
static int rx_buf_pos;
static volatile bool uart_is_busy = true;

/**
 * Read characters from UART until line end is detected. Afterwards push the
 * data to the message queue.
 */
static void serial_cb(const struct device *dev, void *user_data)
{
    uint8_t c;

    if (!uart_irq_update(uart_dev))
    {
        return;
    }

    if (!uart_irq_rx_ready(uart_dev))
    {
        return;
    }

    static char message_tag;

    // Read until FIFO empty
    while (uart_fifo_read(uart_dev, &c, 1) == 1)
    {
        if (rx_buf_pos == -1)
        {
            message_tag = c;
        }

        switch (c)
        {
        case 'b':
            uart_is_busy = true;
            LOG_DBG("UART busy");

            break;

        case 'r':
            uart_is_busy = false;
            LOG_DBG("UART ready");

            break;
        case '\n':
        case '\r':
            // Terminate string
            rx_buf[rx_buf_pos++] = '\0';
            // Send data over BLE
            if (message_tag == 'd')
            {
                bluetooth_write_data(DATA_DEPTH, rx_buf, rx_buf_pos);
                LOG_INF("Depth: %s", rx_buf);
            }
            else if (message_tag == 'p')
            {
                bluetooth_write_data(DATA_PRESSURE, rx_buf, rx_buf_pos);
                LOG_INF("Pressure: %s", rx_buf);
            }
            else if (message_tag == 't')
            {
                bluetooth_write_data(DATA_TEMPERATURE, rx_buf, rx_buf_pos);
                LOG_INF("Temperature: %s", rx_buf);
            }

            // Reset the buffer (it was sendt over BLE)
            // -1 because the tag will be read first and ignored, and then the data will be stored.
            rx_buf_pos = -1;
            break;

        default:
            if (rx_buf_pos == -1)
            {
                rx_buf_pos++;
                break;
            }
            rx_buf[rx_buf_pos++] = c;
        }
    }
}

bool uart_get_is_busy()
{
    return uart_is_busy;
}

void uart_write(char *message)
{

    int msg_len = strlen(message);

    for (int i = 0; i < msg_len; i++)
    {
        uart_poll_out(uart_dev, message[i]);
    }
}

int uart_init()
{
    int ret = 0;

    if (!device_is_ready(uart_dev))
    {

        LOG_ERR("UART device not found!");
        return -EIO;
    }

    const struct uart_config uart_cfg = {
        .baudrate = 56000,
        .parity = UART_CFG_PARITY_NONE,
        .stop_bits = UART_CFG_STOP_BITS_1,
        .data_bits = UART_CFG_DATA_BITS_8,
        .flow_ctrl = UART_CFG_FLOW_CTRL_NONE};

    ret |= uart_configure(uart_dev, &uart_cfg);

    if (ret == -ENOSYS)
    {
        LOG_ERR("UART config failed");
        return -ENOSYS;
    }

    // configure interrupt and callback to receive data
    ret |= uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);

    if (ret == -ENOSYS || ret == -ENOTSUP)
    {
        LOG_ERR("UART callback function not implemented or API not enabled");
        return -ENOSYS;
    }

    uart_irq_rx_enable(uart_dev);
    LOG_INF("Initialized");

    return ret;
}