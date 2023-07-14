import 'dart:async';

import 'package:automatic_deployment_system_app/config/mqtt_topics.dart';
import 'package:automatic_deployment_system_app/controllers/USS_controller.dart';
import 'package:automatic_deployment_system_app/controllers/mqtt_controller.dart';
import 'package:flutter/material.dart';

class SystemController extends ValueNotifier {
  late ValueNotifier<int> currentPage = ValueNotifier(0);
  late USSController underwaterSensorSystem = USSController();
  late MQTTController mqtt = MQTTController();
  double _targetDepth = 6.9;
  int _targetTime = 85;
  int _samplingInterval = 1;

  SystemController() : super(null);

  //CALLBACKS
  List<Function> callbacks = [];
  //NOTIFIER
  ValueNotifier<bool> isOnSystem = ValueNotifier(true);
  ValueNotifier<bool> isSampling = ValueNotifier(true);
  //TIMER
  late Timer systemTimer;
  int systemUpdateIntervalMS = 100;
  int _currentTime = 0;
  int _timeLeft = 0;

  //---------------------------INITIALIZE-------------------------------------
  void initState() {
    underwaterSensorSystem.initState();
    mqtt.connect();

    mqtt.registerCallback((data) {
      underwaterSensorSystem.updateDepth(data);
    });
    mqtt.registerCallback((data) {
      underwaterSensorSystem.updateTemperature(data);
    });
    mqtt.registerCallback((data) {
      underwaterSensorSystem.updatePressure(data);
    });
    mqtt.registerCallback((data) {
      underwaterSensorSystem.updateBattery(data);
    });

    systemTimer = Timer.periodic(
        Duration(milliseconds: systemUpdateIntervalMS), updateFrontendData);
  }

  //---------------------------CALLBACKS-----------------------------------
  void registerCallback(Function cb) {
    callbacks.add(cb);
  }

  //---------------------------GET STATE------------------------------------

  bool getTrue() {
    return true;
  }

  bool getIsOnSystem() {
    return isOnSystem.value;
  }

  bool getIsSampling() {
    if (_timeLeft <= 0) {
      toggleSampling();
    }
    return isSampling.value;
  }

  double getTargetDepth() {
    return _targetDepth;
  }

  int getTargetTime() {
    return _targetTime;
  }

  int getSamplingInterval() {
    return _samplingInterval;
  }

  //---------------------------SET STATE------------------------------------

  void setTargetDepth(double depth) {
    _targetDepth = depth;
  }

  void setTargetTime(int time) {
    _targetTime = time;
  }

  void setSamplingInterval(int interval) {
    _samplingInterval = interval;
  }

  void setDepthInit() {
    mqtt.publishMessage(Topics.depthInit, '1');
  }

  //---------------------------TOGGLE-----------------------------------------

  void toggleSystem() {
    isOnSystem.value = !isOnSystem.value;
    isOnSystem.value
        ? mqtt.publishMessage(Topics.system, '1')
        : mqtt.publishMessage(Topics.system, '0');
    underwaterSensorSystem.resetCharts();

    notifyListeners();
  }

  void toggleSampling() {
    isSampling.value = !isSampling.value;

    if (_timeLeft <= 0) {
      isSampling.value = false;
      _currentTime = 0;
    }

    isSampling.value
        ? mqtt.publishMessage(Topics.sampling, '1')
        : mqtt.publishMessage(Topics.sampling, '0');

    underwaterSensorSystem.resetCharts();
    notifyListeners();
  }

  void toggleRS232() {
    underwaterSensorSystem.toggleRS232();

    underwaterSensorSystem.getIsOnRS232()
        ? mqtt.publishMessage(Topics.rs232, '1')
        : mqtt.publishMessage(Topics.rs232, '0');
  }

  void toggle12V() {
    underwaterSensorSystem.toggle12V();

    underwaterSensorSystem.getIsOn12V()
        ? mqtt.publishMessage(Topics.volt, '1')
        : mqtt.publishMessage(Topics.volt, '0');
  }

  //---------------------------SYSTEM TIMER----------------------------------
  int getCurrentTime() {
    return _currentTime;
  }

  void setTimeLeft(int seconds) {
    _timeLeft = seconds;
  }

  //---------------------------UPDATE DATA-----------------------------------
  void updateFrontendData(Timer timer) {
    if (!isOnSystem.value) _currentTime = 0;

    for (var i = 0; i < callbacks.length; i++) {
      callbacks.elementAt(i)();
    }
  }
}
