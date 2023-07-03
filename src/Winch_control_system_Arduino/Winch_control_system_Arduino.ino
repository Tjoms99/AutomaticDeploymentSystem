#include <WiFi.h>
#include <PubSubClient.h>
extern "C" {
#include "uart.h"
}
#include <stdio.h>
#include <string.h>

//--------------------------------------------------------------------
// WiFi
const char *ssid = "Tjoms";         // Enter your Wi-Fi name
const char *password = "grovt123";  // Enter Wi-Fi password

//--------------------------------------------------------------------
// APPLICATION TOPICS
// Subscribe topics
const char *topic_system = "AutomaticDeploymentSystem/app/system";
const char *topic_sampling = "AutomaticDeploymentSystem/app/sampling";
const char *topic_target_depth = "AutomaticDeploymentSystem/app/target_depth";
const char *topic_target_time = "AutomaticDeploymentSystem/app/target_time";

//--------------------------------------------------------------------
// UNDERWATER SENSOR SYSTEM TOPICS
// Publish topics
const char *topic_depth = "AutomaticDeploymentSystem/USS/depth";
const char *topic_temperature = "AutomaticDeploymentSystem/USS/temperature";
const char *topic_pressure = "AutomaticDeploymentSystem/USS/pressure";
const char *topic_battery = "AutomaticDeploymentSystem/USS/battery";
// Subscribe topics
const char *topic_sampling_interval = "AutomaticDeploymentSystem/USS/sampling_interval";
const char *topic_rs232 = "AutomaticDeploymentSystem/USS/rs232";
const char *topic_12v = "AutomaticDeploymentSystem/USS/12v";

//--------------------------------------------------------------------
// MQTT SETUP
const char *mqtt_broker = "test.mosquitto.org";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

#define MQTT_SUBSCRIBTIONS_LENGTH 7
#define SYSTEM_ON 0x00
#define SAMPLING_ON 0x01
#define RS232_ON 0x02
#define VOLT12_ON 0x03
#define SAMPLING_INTERVAL 0x04
#define TARGET_TIME 0x05
#define TARGET_DEPTH 0x06

char mqtt_data[MQTT_SUBSCRIBTIONS_LENGTH][10];

String client_id = "esp32-client-";
WiFiClient espClient;
PubSubClient client(espClient);

void mqtt_populate_data(int location, byte *payload, unsigned int length) {
  for (int i = 0; i < 10; i++) {
    mqtt_data[location][i] = '\0';
    if (i < length) {
      mqtt_data[location][i] = (char)payload[i];
    }
    Serial.print(mqtt_data[location][i]);
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");

  if (strcmp(topic, topic_system) == 0) {
    mqtt_populate_data(SYSTEM_ON, payload, length);
  } else if (strcmp(topic, topic_sampling) == 0) {
    mqtt_populate_data(SAMPLING_ON, payload, length);
  } else if (strcmp(topic, topic_rs232) == 0) {
    mqtt_populate_data(RS232_ON, payload, length);
  } else if (strcmp(topic, topic_12v) == 0) {
    mqtt_populate_data(VOLT12_ON, payload, length);
  } else if (strcmp(topic, topic_sampling_interval) == 0) {
    mqtt_populate_data(SAMPLING_INTERVAL, payload, length);
    uart_write_sample_time(mqtt_data[SAMPLING_INTERVAL]);
  } else if (strcmp(topic, topic_target_depth) == 0) {
    mqtt_populate_data(TARGET_TIME, payload, length);
  } else if (strcmp(topic, topic_target_time) == 0) {
    mqtt_populate_data(TARGET_DEPTH, payload, length);
  }

  Serial.println();
  Serial.println("-----------------------");
}

char *get_rx_buffer() {
  char *buffer = "test";
  uint8_t length = 1;

  uart_read(&buffer);
  Serial.println(buffer);

  return buffer;
}

void assign_data(char temperature[6], char pressure[6], char depth[6]) {
  static const uint8_t data_per_attribute = 1 + 1 + 6;  // NEW LINE + TAG + DATA
  static const uint8_t data_elements = 3;               // Temperature, pressure, depth

  static const char temperature_tag = 't';
  static const char pressure_tag = 'p';
  static const char depth_tag = 'd';

  char *data = get_rx_buffer();
  Serial.println(data);

  for (int data_index = 0; data_index < (data_per_attribute * data_elements); data_index++) {
    if (temperature_tag == data[data_index]) {
      temperature[0] = data[data_index + 1];
      temperature[1] = data[data_index + 2];
      temperature[2] = data[data_index + 3];
      temperature[3] = data[data_index + 4];
      temperature[4] = data[data_index + 5];
      temperature[5] = data[data_index + 6];
      data_index += data_per_attribute;
    }

    if (pressure_tag == data[data_index]) {
      pressure[0] = data[data_index + 1];
      pressure[1] = data[data_index + 2];
      pressure[2] = data[data_index + 3];
      pressure[3] = data[data_index + 4];
      pressure[4] = data[data_index + 5];
      pressure[5] = data[data_index + 6];
      data_index += data_per_attribute;
    }

    if (depth_tag == data[data_index]) {
      depth[0] = data[data_index + 1];
      depth[1] = data[data_index + 2];
      depth[2] = data[data_index + 3];
      depth[3] = data[data_index + 4];
      depth[4] = data[data_index + 5];
      depth[5] = data[data_index + 6];
      data_index += data_per_attribute;
    }
  }

  Serial.println(temperature);
  Serial.println(pressure);
  Serial.println(depth);
}
void publish_data() {
  static char temperature[6] = "0";
  static char pressure[6] = "0";
  static char depth[6] = "0";

  assign_data(temperature, pressure, depth);

  client.publish(topic_depth, depth, true);
  client.publish(topic_temperature, temperature, true);
  client.publish(topic_pressure, pressure, true);
  client.publish(topic_battery, "69", true);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void setup() {

  // For the USB, just use Serial as normal:
  Serial.begin(115200);

  // Connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  uart_begin();

  // connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  client_id += String(WiFi.macAddress());
  while (!client.connected()) {
    Serial.println("Connecting..");
    client.connect(client_id.c_str(), mqtt_username, mqtt_password);
    delay(2000);
  }
  //  subscribe
  client.subscribe(topic_system);
  client.subscribe(topic_sampling);
  client.subscribe(topic_sampling_interval);
  client.subscribe(topic_rs232);
  client.subscribe(topic_12v);
  client.subscribe(topic_target_depth);
  client.subscribe(topic_target_time);

  uart_write_reset();
  uart_write_sample();
}

void loop() {
  client.loop();
  // If connection failed, or connection lost, or connection timeout, retry the connection.
  if (client.state() < 0) {
    client.connect(client_id.c_str(), mqtt_username, mqtt_password);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  if (mqtt_data[SAMPLING_ON][0] == '1') {
    publish_data();
  }
}