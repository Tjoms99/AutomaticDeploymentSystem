#include "driver/uart.h"
#include "esp_intr_alloc.h"
#include "uart.h"
#include "Arduino.h"

uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
};

static const uart_port_t UART_NUM = UART_NUM_0;
static const int RX_BUFFER_SIZE = (1024 * 2);

static char *rx_data = "\nUDEF";

static uart_callback_data_ready callbacks_data[10];
static uint8_t callback_data_n = 0;

static void uart_assign_data(char *data)
{
  // TODO: Implement motor position feedback
  //  https://robokits.download/downloads/RMCS220x_DCServo_Driver.pdf
}

// Not needed as depth is used?
static void uart_rx(void *arg)
{
  uint8_t *data = (uint8_t *)malloc(RX_BUFFER_SIZE + 1);
  int length = 0;

  uart_flush(UART_NUM);

  while (1)
  {
    uart_get_buffered_data_len(UART_NUM, (size_t *)&length);
    length = uart_read_bytes(UART_NUM, data, length, 100);
    uart_assign_data(data);

    if (length > 10)
    {
      uart_flush_input(UART_NUM);
      for (uint8_t callback = 0; callback < callback_data_n; callback++)
      {
        callbacks_data[callback]();
      }
    }

    // Dont poll in the future
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void uart_begin(void)
{
  uart_param_config(UART_NUM, &uart_config);
  uart_set_pin(UART_NUM, D6, D7, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  uart_driver_install(UART_NUM, RX_BUFFER_SIZE, 0, 10, NULL, 0);

  xTaskCreate(uart_rx, "uart_rx_task", 2048, NULL, 10, NULL);
}

void uart_write_char(char *buffer)
{
  uart_write_bytes(UART_NUM, (const char *)buffer, strlen(buffer));
}

void uart_register_callback_data_ready(uart_callback_data_ready cb)
{
  callbacks_data[callback_data_n++] = cb;
}