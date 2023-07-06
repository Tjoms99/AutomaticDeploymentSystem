extern "C" {
#include "uart.h"
}
#include <WiFi.h>
//#include <stdio.h>
//#include <string.h>

//--------------------------------------------------------------------
// WiFi
const char *ssid = "Tjoms";         // Enter your Wi-Fi name
const char *password = "grovt123";  // Enter Wi-Fi password

static const int USS_ENABLED = D0;

void setup() {

  pinMode(USS_ENABLED, OUTPUT);
  digitalWrite(USS_ENABLED, HIGH);

  // For the USB, just use Serial as normal:
  Serial.begin(115200);

  uart_begin();
  WiFi.begin(ssid, password);
  ble_begin();
  mqtt_begin();

  uart_register_callback_data_ready(publish_data);
  uart_write_reset();
}

void loop() {
  //vTaskDelay(1000 / portTICK_PERIOD_MS);
}