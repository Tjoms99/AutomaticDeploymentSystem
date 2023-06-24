import "package:automatic_deployment_system_app/config/mqtt_topics.dart";
import 'package:automatic_deployment_system_app/controllers/USS_controller.dart';
import "package:flutter/material.dart";
import "package:mqtt_client/mqtt_client.dart";
import "package:mqtt_client/mqtt_server_client.dart";

const String mqttBroker = "test.mosquitto.org";
const String mqttClientName = "";
const int mqttBrokerPort = 1883;

class MQTTController with ChangeNotifier {
  // Value notifiers
  final ValueNotifier<String> depth = ValueNotifier<String>('0');
  final ValueNotifier<String> temperature = ValueNotifier<String>('0');
  final ValueNotifier<String> pressure = ValueNotifier<String>('0');

  MQTTController();
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
    client.subscribe(Topics.battery, MqttQos.atLeastOnce);

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
          // USS.getDepthSensor().updateCurrentData(double.parse(stringMsg));
          break;
        case Topics.temperature:
          //USS.getTemperatureSensor().updateCurrentData(double.parse(stringMsg));
          break;
        case Topics.pressure:
          //USS.getPressureSensor().updateCurrentData(double.parse(stringMsg));
          break;

        default:
      }

      // print('MQTT | Data received on topic: $topic');
      //  print('MQTT | Payload size = ${message.length}');
      // print('');
    });

    return client;
  }

  void onConnected() {
    print('MQTT | Connected');
  }

  void onDisconnected() {
    print('MQTT | Disconnected');
    client.connect();
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
    if (client.connectionStatus!.state == MqttConnectionState.disconnected) {
      print('MQTT | Client Disconnected');
      return;
    }

    final builder = MqttClientPayloadBuilder();
    builder.addString(message);
    client.publishMessage(topic, MqttQos.atLeastOnce, builder.payload!);
  }

  //---------------------------ADS FUNCTIONS-----------------------------------
/*
  void toggleSystem() {
    USS.toggleSystem();
    publishMessage(Topics.system, USS.getIsOnSystem().toString());
  }

  void toggleSampling() {
    USS.toggleSampling();
    publishMessage(Topics.sampling, USS.getIsSampling().toString());
  }

  void toggleRS232() {
    USS.toggleRS232();
    publishMessage(Topics.rs232, USS.getIsOnRS232().toString());
  }

  void toggle12V() {
    USS.toggle12V();
    publishMessage(Topics.volt, USS.getIsOn12V().toString());
  }
  */
}
