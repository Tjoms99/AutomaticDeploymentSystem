import "package:automatic_deployment_system_app/config/mqtt_topics.dart";
import 'package:automatic_deployment_system_app/controllers/USS_controller.dart';
import "package:flutter/material.dart";
import "package:mqtt_client/mqtt_client.dart";
import "package:mqtt_client/mqtt_server_client.dart";

const String mqttBroker = "test.mosquitto.org";
const String mqttClientName = "";
const int mqttBrokerPort = 1883;

typedef MQTTCallback = void Function(String data);

class MQTTController extends ValueNotifier {
  ValueNotifier<bool> isConnected = ValueNotifier(false);

  // Value notifiers
  String depth = '0';
  String temperature = '0';
  String pressure = '0';
  String battery = '0';

  MQTTController() : super(null);
  // Client to be initialized
  late MqttServerClient client;

  //CALLBACKS
  List<MQTTCallback> callbacks = [];

  //---------------------------CALLBACKS-----------------------------------
  void registerCallback(MQTTCallback cb) {
    callbacks.add(cb);
  }

  //---------------------------MQTT FUNCTIONS-----------------------------------

  void subscribeToTopics() {
    if (client.connectionStatus!.state != MqttConnectionState.connected) return;

    client.subscribe(Topics.depth, MqttQos.atLeastOnce);
    client.subscribe(Topics.temperature, MqttQos.atLeastOnce);
    client.subscribe(Topics.pressure, MqttQos.atLeastOnce);
    client.subscribe(Topics.battery, MqttQos.atLeastOnce);
  }

  void updateData(String topic, String message) {
    try {
      switch (topic) {
        case Topics.depth:
          depth = message;
          callbacks.elementAt(SensorType.DEPTH.index)(depth);
          break;

        case Topics.temperature:
          temperature = message;
          callbacks.elementAt(SensorType.TEMPERATURE.index)(temperature);
          break;

        case Topics.pressure:
          pressure = message;
          callbacks.elementAt(SensorType.PRESSURE.index)(pressure);
          break;

        case Topics.battery:
          battery = message;
          callbacks.elementAt(SensorType.BATTERY.index)(battery);
          break;

        default:
      }
    } catch (e) {
      print("${e}");
      print(
          "Tried to write message ${message} to topic ${topic}. Message cannot be converted to double");
    }

    for (var i = 0; i < callbacks.length; i++) {}
  }

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

    subscribeToTopics();
    // Check if connected, if not disconnect properly
    if (client.connectionStatus!.state == MqttConnectionState.connected) {
      print('MQTT | Client connected');
    } else {
      print(
          'MQTT | ERROR client connection failed - disconnecting, status is ${client.connectionStatus}');
      client.disconnect();
      return -1;
    }

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

      updateData(topic, stringMsg);

      // print('MQTT | Data received on topic: $topic');
      //  print('MQTT | Payload size = ${message.length}');
      // print('');
    });

    return client;
  }

  void disconnect() {
    client.disconnect();
  }

  void onConnected() {
    print('MQTT | Connected');
    isConnected.value = true;
  }

  void onDisconnected() {
    print('MQTT | Disconnected');
    isConnected.value = false;
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
    client.publishMessage(topic, MqttQos.atLeastOnce, builder.payload!,
        retain: true);
  }
}
