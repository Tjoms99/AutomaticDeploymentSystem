#include "driver/uart.h"
#include "esp_intr_alloc.h"
#include "uart.h"
#include "Arduino.h"

uart_config_t uart_config = {
    .baud_rate = 56000,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
};

const uart_port_t UART_NUM = UART_NUM_0;
const int RX_BUFFER_SIZE = (1024 * 2);
const int RS485_TX_EN_PIN = D5;

char *rx_data = "\nUDEF";

static void uart_test(void *arg)
{
    char *buffer = "\n5\n";

    while (1)
    {
        digitalWrite(RS485_TX_EN_PIN, HIGH);
        uart_write_bytes(UART_NUM, (const char *)buffer, strlen(buffer));
        vTaskDelay(1 * strlen(buffer) / portTICK_PERIOD_MS);
        digitalWrite(RS485_TX_EN_PIN, LOW);

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

static void uart_rx(void *arg)
{
    // uint8_t data[128];
    uint8_t *data = (uint8_t *)malloc(RX_BUFFER_SIZE + 1);
    int length = 0;

    uart_flush(UART_NUM);

    while (1)
    {
        uart_get_buffered_data_len(UART_NUM, (size_t *)&length);
        length = uart_read_bytes(UART_NUM, data, length, 100);
        if (length > 0)
        {

            rx_data = data;
            // rx_data[0] = "Q";

            // uart_write(rx_data);
            uart_flush_input(UART_NUM);
        }

        else if (length < 0)
        {
            uart_write("\nERR");
        }
        else
        {
            uart_write("\nNOP");
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void uart_begin(void)
{
    // Configure UART parameters
    pinMode(RS485_TX_EN_PIN, OUTPUT);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, D6, D7, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uart_driver_install(UART_NUM, RX_BUFFER_SIZE, 0, 10, NULL, 0);

    uart_flush(UART_NUM);
    // xTaskCreate(uart_test, "uart_test_task", 2048, NULL, 10, NULL);
    xTaskCreate(uart_rx, "uart_rx_task", 2048, NULL, 10, NULL);
}

void uart_write(char *buffer)
{
    digitalWrite(RS485_TX_EN_PIN, HIGH);
    uart_write_bytes(UART_NUM, (const char *)buffer, strlen(buffer));
    vTaskDelay(1 / portTICK_PERIOD_MS);
    digitalWrite(RS485_TX_EN_PIN, LOW);
}

void uart_read(char **buffer)
{
    *buffer = rx_data;
}