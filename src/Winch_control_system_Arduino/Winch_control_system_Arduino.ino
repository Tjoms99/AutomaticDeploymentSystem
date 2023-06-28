#include <WiFi.h>
#include <PubSubClient.h>
extern "C" {
#include "uart.h"
}

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

char mqttData[MQTT_SUBSCRIBTIONS_LENGTH][10];

String client_id = "esp32-client-";
WiFiClient espClient;
PubSubClient client(espClient);

void mqtt_populate_data(int location, byte *payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    mqttData[location][i] = (char)payload[i];
    Serial.print(mqttData[location][i]);
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

void publish_data() {
  static int depth = -50;
  if (depth > 0) {
    depth = -50;
  }
  char *data = get_rx_buffer();
  char temperature[5];
  temperature[0] = data[1];
  temperature[1] = data[2];
  temperature[2] = data[3];
  temperature[3] = data[4];
  temperature[4] = data[5];
  temperature[5] = data[6];



  client.publish(topic_depth, (char *)String(depth++, DEC).c_str(), true);
  client.publish(topic_temperature, temperature, true);
  client.publish(topic_pressure, "101000", true);
  client.publish(topic_battery, "69", true);

  delay(1000);
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
}

void loop() {
  client.loop();
  // If connection failed, or connection lost, or connection timeout, retry the connection.
  if (client.state() < 0) {
    client.connect(client_id.c_str(), mqtt_username, mqtt_password);
    delay(2000);
  }

  if (mqttData[SAMPLING_ON][0] == '1') {
    publish_data();
  }
}