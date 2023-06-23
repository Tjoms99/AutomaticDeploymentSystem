import "package:automatic_deployment_system_app/controllers/underwater_sensor_system.dart";
import "package:flutter/material.dart";
import "package:mqtt_client/mqtt_client.dart";
import "package:mqtt_client/mqtt_server_client.dart";

const String mqttBroker = "test.mosquitto.org";
const String mqttClientName = "";
const int mqttBrokerPort = 1883;

// MQTT Topics
abstract class Topics {
  // Controll
  static const String system = "AutomaticDeploymentSystem/app/system";
  static const String sampling = "AutomaticDeploymentSystem/app/sampling";
  static const String rs232 = "AutomaticDeploymentSystem/app/rs232";
  static const String volt = "AutomaticDeploymentSystem/app/12v";

  // Data
  static const String temperature = "AutomaticDeploymentSystem/USS/temperature";
  static const String pressure = "AutomaticDeploymentSystem/USS/pressure";
  static const String depth = "AutomaticDeploymentSystem/USS/depth";
}

class MQTTController with ChangeNotifier {
  // Value notifiers
  final ValueNotifier<String> depth = ValueNotifier<String>('0');
  final ValueNotifier<String> temperature = ValueNotifier<String>('0');
  final ValueNotifier<String> pressure = ValueNotifier<String>('0');

  UnderwaterSensorSystemController underwaterSensorSystem;

  MQTTController(this.underwaterSensorSystem);
  // Client to be initialized
  late MqttServerClient client;

  //---------------------------MQTT FUNCTIONS-----------------------------------
  Future<Object> connect() async {
    client =
        MqttServerClient.withPort(mqttBroker, mqttClientName, mqttBrokerPort);
    client.logging(on: false);
    client.onConnected = onConnected;
    client.onDisconnected = onDisconnected;
    client.onSubscribed = onSubscribed;
    client.onSubscribeFail = onSubscribeFail;
    client.pongCallback = pong;
    client.keepAlivePeriod = 60;
    client.logging(on: false);
    client.setProtocolV311();

    // Configure connection will-message and set clean session
    final connMessage = MqttConnectMessage()
        .withWillTopic('willtopic')
        .withWillMessage('Will message')
        .startClean()
        .withWillQos(MqttQos.atLeastOnce);

    // Connect to broker
    print('MQTT | Client connecting....');
    client.connectionMessage = connMessage;
    try {
      await client.connect();
    } catch (e) {
      print('Exception: $e');
      client.disconnect();
    }

    // Check if connected, if not disconnect properly
    if (client.connectionStatus!.state == MqttConnectionState.connected) {
      print('MQTT | Client connected');
    } else {
      print(
          'MQTT | ERROR client connection failed - disconnecting, status is ${client.connectionStatus}');
      client.disconnect();
      return -1;
    }

    // Subscribe to topics
    client.subscribe(Topics.depth, MqttQos.atLeastOnce);
    client.subscribe(Topics.temperature, MqttQos.atLeastOnce);
    client.subscribe(Topics.pressure, MqttQos.atLeastOnce);

    // Listen for messages on subscribed topics
    client.updates!.listen((List<MqttReceivedMessage<MqttMessage?>>? msg) {
      // Convert payload to MqttPublishMessage
      final data = msg![0].payload as MqttPublishMessage;

      // Get Uint8Buffer message
      var message = data.payload.message;

      // Convert Uint8Buffer to String
      String stringMsg = MqttPublishPayload.bytesToStringAsString(message);

      // Get topic
      String topic = msg[0].topic;

      // Filter topics and update corresponding ValueNotifier
      switch (topic) {
        case Topics.depth:
          underwaterSensorSystem
              .getDepthSensor()
              .updateCurrentData(double.parse(stringMsg));
          break;
        case Topics.temperature:
          underwaterSensorSystem
              .getTemperatureSensor()
              .updateCurrentData(double.parse(stringMsg));
          break;
        case Topics.pressure:
          underwaterSensorSystem
              .getPressureSensor()
              .updateCurrentData(double.parse(stringMsg));
          break;

        default:
      }

      print('MQTT | Data received on topic: $topic');
      print('MQTT | Payload size = ${message.length}');
      print('');
    });

    return client;
  }

  void onConnected() {
    print('MQTT | Connected');
  }

  void onDisconnected() {
    print('MQTT | Disconnected');
  }

  void onSubscribed(String topic) {
    print('MQTT | Subscribed topic: $topic');
  }

  void onSubscribeFail(String topic) {
    print('MQTT | Failed to subscribe $topic');
  }

  void onUnsubscribed(String? topic) {
    print('MQTT | Unsubscribed topic: $topic');
  }

  void pong() {
    print('MQTT | Ping response client callback invoked');
  }

  void publishMessage(String topic, String message) {
    final builder = MqttClientPayloadBuilder();
    builder.addString(message);
    client.publishMessage(topic, MqttQos.atLeastOnce, builder.payload!);
  }

  //---------------------------ADS FUNCTIONS-----------------------------------

  void toggleSystem() {
    underwaterSensorSystem.toggleSystem();
    publishMessage(
        Topics.system, underwaterSensorSystem.getIsOnSystem().toString());
  }

  void toggleSampling() {
    underwaterSensorSystem.toggleSampling();
    publishMessage(
        Topics.sampling, underwaterSensorSystem.getIsSampling().toString());
  }

  void toggleRS232() {
    underwaterSensorSystem.toggleRS232();
    publishMessage(
        Topics.rs232, underwaterSensorSystem.getIsOnRS232().toString());
  }

  void toggle12V() {
    underwaterSensorSystem.toggle12V();
    publishMessage(Topics.volt, underwaterSensorSystem.getIsOn12V().toString());
  }
}
