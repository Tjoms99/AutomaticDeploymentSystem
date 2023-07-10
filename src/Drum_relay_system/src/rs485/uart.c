#include "uart.h"
#include "../bluetooth/bluetooth.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <string.h>

static const struct device *const uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));

#define MSG_SIZE 16
static char rx_buf[MSG_SIZE];
static int rx_buf_pos;
static volatile bool uart_is_busy = true;

static uart_callback_uss_ready callbacks_data[10];
static uint8_t callback_data_n = 0;

/* queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uart_msgq_depth, MSG_SIZE, 10, 4);
K_MSGQ_DEFINE(uart_msgq_pressure, MSG_SIZE, 10, 4);
K_MSGQ_DEFINE(uart_msgq_temperature, MSG_SIZE, 10, 4);

/*
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

    /* read until FIFO empty */
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
            break;

        case 'r':
            uart_is_busy = false;
            break;
        case '\n':
        case '\r':
            /* terminate string */
            rx_buf[rx_buf_pos++] = '\0';

            /* if queue is full, message is silently dropped */
            if (message_tag == 'd')
            {
                bluetooth_write_data(DATA_DEPTH, &rx_buf, rx_buf_pos);
            }
            else if (message_tag == 'p')
            {
                bluetooth_write_data(DATA_PRESSURE, &rx_buf, rx_buf_pos);
            }
            else if (message_tag == 't')
            {
                bluetooth_write_data(DATA_TEMPERATURE, &rx_buf, rx_buf_pos);
            }

            /* reset the buffer (it was copied to the msgq) */
            rx_buf_pos = -1;
            printk("\n\r");
            break;
        default:
            if (rx_buf_pos == -1)
            {
                rx_buf_pos++;
                break;
            }
            rx_buf[rx_buf_pos++] = c;
            printk("%c", c);
        }

        /* else: characters beyond buffer size are dropped */
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

void uart_init()
{
    if (!device_is_ready(uart_dev))
    {
        printk("UART device not found!");
        return;
    }

    /* configure interrupt and callback to receive data */
    int ret = uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);
    uart_irq_rx_enable(uart_dev);
}