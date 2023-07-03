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

static void uart_rx(void *arg) {
  uint8_t *data = (uint8_t *)malloc(RX_BUFFER_SIZE + 1);
  int length = 0;

  uart_flush(UART_NUM);

  while (1) {
    uart_get_buffered_data_len(UART_NUM, (size_t *)&length);
    length = uart_read_bytes(UART_NUM, data, length, 100);
    if (length > 0) {
      vTaskDelay(10 / portTICK_PERIOD_MS);
      rx_data = data;
      uart_flush_input(UART_NUM);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void uart_begin(void) {
  // Configure UART parameters
  pinMode(RS485_TX_EN_PIN, OUTPUT);
  uart_param_config(UART_NUM, &uart_config);
  uart_set_pin(UART_NUM, D6, D7, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  uart_driver_install(UART_NUM, RX_BUFFER_SIZE, 0, 10, NULL, 0);

  xTaskCreate(uart_rx, "uart_rx_task", 2048, NULL, 10, NULL);
}

void uart_write_char(char *buffer) {
  digitalWrite(RS485_TX_EN_PIN, HIGH);
  uart_write_bytes(UART_NUM, (const char *)buffer, strlen(buffer));
  vTaskDelay(1 / portTICK_PERIOD_MS);
  digitalWrite(RS485_TX_EN_PIN, LOW);
}

void uart_write_reset() {
  uart_write_char("7");
}

void uart_write_sample() {
  uart_write_char("45");
}

void uart_write_init_depth() {
  uart_write_char("3");
}

void uart_write_sample_time(char *sampling_interval) {

  char buffer[3];
  char *time = buffer;

  time[0] = sampling_interval[2];
  time[1] = sampling_interval[1];
  time[2] = sampling_interval[0];

  if (time[2] == '\0') {
    time[2] = 48;
  }
  if (time[1] == '\0') {
    time[1] = 48;
  }
  if (time[0] == '\0') {
    time[0] = 48;
  }

  uart_write_char("4");
  uart_write_char("2");
  uart_write_char(time);
  uart_write_char("4");
}

void uart_write_rs232() {
  vTaskDelay(1 / portTICK_PERIOD_MS);
}

void uart_write_12v() {
  uart_write_char("0");
}

void uart_read(char **buffer) {
  *buffer = rx_data;
}