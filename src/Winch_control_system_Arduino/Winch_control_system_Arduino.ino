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

static const int USS_ENABLED = D0;
volatile bool ble_finised = false;
void setup()
{

  pinMode(USS_ENABLED, OUTPUT);
  digitalWrite(USS_ENABLED, HIGH);

  // For the USB, just use Serial as normal:
  Serial.begin(115200);
  Serial.println("Before init");

  ble_begin();
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  WiFi.begin(ssid, password);
  mqtt_begin();

  Serial.println("Initialized");
}

void loop()
{
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  ble_loop();
}