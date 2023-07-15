extern "C"
{
#include "uart.h"
}
#include <WiFi.h>
// #include <stdio.h>
// #include <string.h>

//--------------------------------------------------------------------
// WiFi
const char *ssid = "Tjoms";        // Enter your Wi-Fi name
const char *password = "grovt123"; // Enter Wi-Fi password

void setup()
{
  // For the USB, just use Serial as normal:
  Serial.begin(115200);
  Serial.println("Before init");

  ble_begin();
  vTaskDelay(3000 / portTICK_PERIOD_MS);

  WiFi.begin(ssid, password);
  mqtt_begin();

  Serial.println("Initialized");
}

void loop()
{
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}