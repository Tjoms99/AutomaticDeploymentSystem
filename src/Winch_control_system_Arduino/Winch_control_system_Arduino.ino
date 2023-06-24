#include <WiFi.h>
#include <PubSubClient.h>
#include <math.h>

// WiFi
const char *ssid = "Tjoms";         // Enter your Wi-Fi name
const char *password = "grovt123";  // Enter Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org";

const char *topic_system = "AutomaticDeploymentSystem/app/system";
const char *topic_sampling = "AutomaticDeploymentSystem/app/sampling";
const char *topic_depth = "AutomaticDeploymentSystem/USS/depth";
const char *topic_temperature = "AutomaticDeploymentSystem/USS/temperature";
const char *topic_pressure = "AutomaticDeploymentSystem/USS/pressure";
const char *topic_battery = "AutomaticDeploymentSystem/USS/battery";

const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

char isSampling[1] = { 'f' };

WiFiClient espClient;
PubSubClient client(espClient);


void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  isSampling[0] = (char)payload[0];
  Serial.println();
  Serial.println("-----------------------");
}

void publishData() {
  static int depth = -50;
  client.publish(topic_depth, (char*)String(depth++, DEC).c_str());
  client.publish(topic_temperature, "20");
  client.publish(topic_pressure, "101000");
  client.publish(topic_battery, "69");

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

  String client_id = "esp32-client-";
  client_id += String(WiFi.macAddress());
  while (!client.connected()) {
    client.connect(client_id.c_str(), mqtt_username, mqtt_password);
    delay(2000);
  }
  //  subscribe
  client.subscribe(topic_sampling);
}

void loop() {
  client.loop();
  if (isSampling[0] == 't') {
    publishData();
  }
}