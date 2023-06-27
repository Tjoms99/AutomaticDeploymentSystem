#include <WiFi.h>
#include <PubSubClient.h>
#include <math.h>

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

void populateData(int location, byte *payload, unsigned int length) {
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
    populateData(SYSTEM_ON, payload, length);
  } else if (strcmp(topic, topic_sampling) == 0) {
    populateData(SAMPLING_ON, payload, length);
  } else if (strcmp(topic, topic_rs232) == 0) {
    populateData(RS232_ON, payload, length);
  } else if (strcmp(topic, topic_12v) == 0) {
    populateData(VOLT12_ON, payload, length);
  } else if (strcmp(topic, topic_sampling_interval) == 0) {
    populateData(SAMPLING_INTERVAL, payload, length);
  }else if (strcmp(topic, topic_target_depth) == 0) {
    populateData(TARGET_TIME, payload, length);
  }else if (strcmp(topic, topic_target_time) == 0) {
    populateData(TARGET_DEPTH, payload, length);
  }

  Serial.println();
  Serial.println("-----------------------");
}

void publishData() {
  static int depth = -50;
  if (depth > 0) {
    depth = -50;
  }
  client.publish(topic_depth, (char *)String(depth++, DEC).c_str(), true);
  client.publish(topic_temperature, "20", true);
  client.publish(topic_pressure, "101000", true);
  client.publish(topic_battery, "69", true);

  delay(1000);
}

void setup() {
  // Set software serial baud to 115200;
  Serial.begin(115200);
  // Connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  //connecting to a mqtt broker
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
  //If connection failed, or connection lost, or connection timeout, retry the connection.
  if (client.state() < 0) {
    client.connect(client_id.c_str(), mqtt_username, mqtt_password);
    delay(2000);
  }

  if (mqttData[SAMPLING_ON][0] == '1') {
    publishData();
  }
}