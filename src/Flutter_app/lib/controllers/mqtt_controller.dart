import "package:automatic_deployment_system_app/config/mqtt_topics.dart";
import "package:flutter/material.dart";
import "package:mqtt_client/mqtt_client.dart";
import "package:mqtt_client/mqtt_server_client.dart";

const String mqttBroker = "test.mosquitto.org";
const String mqttClientName = "";
const int mqttBrokerPort = 1883;

typedef MQTTCallback = void Function(String data);

class MQTTController extends ValueNotifier {
  ValueNotifier<bool> isConnected = ValueNotifier(false);

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

    client.subscribe(Topics.system, MqttQos.atLeastOnce);
    client.subscribe(Topics.sampling, MqttQos.atLeastOnce);
    client.subscribe(Topics.rs232, MqttQos.atLeastOnce);
    client.subscribe(Topics.volt, MqttQos.atLeastOnce);

    client.subscribe(Topics.targetDepth, MqttQos.atLeastOnce);
    client.subscribe(Topics.targetTime, MqttQos.atLeastOnce);
    client.subscribe(Topics.samplingInterval, MqttQos.atLeastOnce);

    client.subscribe(Topics.depth, MqttQos.atLeastOnce);
    client.subscribe(Topics.temperature, MqttQos.atLeastOnce);
    client.subscribe(Topics.pressure, MqttQos.atLeastOnce);
    client.subscribe(Topics.battery, MqttQos.atLeastOnce);
  }

  void updateData(String topic, String message) {
    try {
      switch (topic) {
        case Topics.system:
          callbacks.elementAt(TopicsE.SYSTEM.index)(message);
          break;
        case Topics.sampling:
          callbacks.elementAt(TopicsE.SAMPLING.index)(message);
          break;
        case Topics.rs232:
          callbacks.elementAt(TopicsE.RS232.index)(message);
          break;
        case Topics.volt:
          callbacks.elementAt(TopicsE.VOLT.index)(message);
          break;
        case Topics.targetDepth:
          callbacks.elementAt(TopicsE.TARGET_DEPTH.index)(message);
          break;
        case Topics.targetTime:
          callbacks.elementAt(TopicsE.TARGET_TIME.index)(message);
          break;
        case Topics.samplingInterval:
          callbacks.elementAt(TopicsE.SAMPLING_INTERVAL.index)(message);
          break;
        case Topics.depth:
          callbacks.elementAt(TopicsE.DEPTH.index)(message);
          break;
        case Topics.temperature:
          callbacks.elementAt(TopicsE.TEMPERATURE.index)(message);
          break;

        case Topics.pressure:
          callbacks.elementAt(TopicsE.PRESSURE.index)(message);
          break;

        case Topics.battery:
          callbacks.elementAt(TopicsE.BATTERY.index)(message);
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
    if (!(client.connectionStatus!.state == MqttConnectionState.connected)) {
      print('MQTT | Client Disconnected');
      return;
    }

    final builder = MqttClientPayloadBuilder();
    builder.addString(message);
    client.publishMessage(topic, MqttQos.atLeastOnce, builder.payload!,
        retain: true);
  }
}
