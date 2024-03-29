#include <PubSubClient.h>
#include <WiFi.h>

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
const char *topic_depth_init = "AutomaticDeploymentSystem/USS/depth_init";
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

void mqtt_populate_data(int location, byte *payload, unsigned int length)
{
  for (int i = 0; i < 10; i++)
  {
    mqtt_data[location][i] = '\0';
    if (i < length)
    {
      mqtt_data[location][i] = (char)payload[i];
    }
    Serial.print(mqtt_data[location][i]);
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");

  if (strcmp(topic, topic_system) == 0)
  {
    ble_notify_system_on(payload, length);
  }
  else if (strcmp(topic, topic_sampling) == 0)
  {

    ble_notify_sampling_on(payload, length);
  }
  else if (strcmp(topic, topic_depth_init) == 0)
  {
    ble_notify_depth_init(payload, length);
  }
  else if (strcmp(topic, topic_rs232) == 0)
  {
    ble_notify_rs232_on(payload, length);
  }
  else if (strcmp(topic, topic_12v) == 0)
  {
    ble_notify_12v_on(payload, length);
  }
  else if (strcmp(topic, topic_sampling_interval) == 0)
  {
    ble_notify_sampling_time(payload, length);
  }
  else if (strcmp(topic, topic_target_depth) == 0)
  {
    //ADD
  }
  else if (strcmp(topic, topic_target_time) == 0)
  {
  }

  Serial.println();
  Serial.println("-----------------------");
}
void publish_depth(const char *depth)
{
  client.publish(topic_depth, depth, true);
}

void publish_pressure(const char *pressure)
{
  client.publish(topic_pressure, pressure, true);
}

void publish_temperature(const char *temperature)
{
  client.publish(topic_temperature, temperature, true);
}

void publish_battery(const char *battery)
{
  client.publish(topic_battery, battery, true);
}

void publish_data(char temperature[6], char pressure[6], char depth[6])
{
  client.publish(topic_depth, depth, true);
  client.publish(topic_temperature, temperature, true);
  client.publish(topic_pressure, pressure, true);
  client.publish(topic_battery, "69", true);
}

void mqtt_task(void *arg)
{
  while (1)
  {
    client.loop();
    // If connection failed, or connection lost, or connection timeout, retry the connection.
    if (client.state() < 0)
    {
      client.connect(client_id.c_str(), mqtt_username, mqtt_password);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void mqtt_begin()
{
  Serial.println("MQTT STARTED!");

  // connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  client_id += String(WiFi.macAddress());
  while (!client.connected())
  {
    Serial.println("Connecting..");
    client.connect(client_id.c_str(), mqtt_username, mqtt_password);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

  Serial.println("Connected!");
  //  subscribe
  client.subscribe(topic_sampling);
  client.subscribe(topic_sampling_interval);
  client.subscribe(topic_depth_init);
  client.subscribe(topic_rs232);
  client.subscribe(topic_12v);
  client.subscribe(topic_target_depth);
  client.subscribe(topic_target_time);
  client.subscribe(topic_system); //!!MUST be last. Used for sync!!


  xTaskCreate(mqtt_task, "mqtt_task", 2048, NULL, 9, NULL);
}