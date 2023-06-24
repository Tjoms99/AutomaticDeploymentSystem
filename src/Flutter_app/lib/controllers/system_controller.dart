import 'dart:async';

import 'package:automatic_deployment_system_app/config/mqtt_topics.dart';
import 'package:automatic_deployment_system_app/controllers/USS_controller.dart';
import 'package:automatic_deployment_system_app/controllers/mqtt_controller.dart';
import 'package:flutter/material.dart';

class SystemController extends ValueNotifier {
  late ValueNotifier<int> currentPage = ValueNotifier(0);
  late USSController underwaterSensorSystem = USSController();
  late MQTTController mqtt = MQTTController();

  SystemController() : super(null);

  //CALLBACKS
  List<UnderwaterSensorSystemCallback> callbacks = [];
  //NOTIFIER
  ValueNotifier<bool> isOnSystem = ValueNotifier(true);
  ValueNotifier<bool> isSampling = ValueNotifier(true);
  //TIMER
  int _samplingInterval = 1;
  late Timer _systemTimer;
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

    _systemTimer = Timer.periodic(
        Duration(milliseconds: systemUpdateIntervalMS), updateFrontendData);
  }

  //---------------------------CALLBACKS-----------------------------------
  void registerCallback(UnderwaterSensorSystemCallback cb) {
    callbacks.add(cb);
  }

  //---------------------------GET STATE------------------------------------

  bool getIsOnSystem() {
    return isOnSystem.value;
  }

  bool getIsSampling() {
    if (_timeLeft <= 0) {
      isSampling.value = false;
      _currentTime = 0;
    }

    return isSampling.value;
  }

  int getSamplingInterval() {
    return _samplingInterval;
  }

  //---------------------------TOGGLE-----------------------------------------

  void toggleSystem() {
    isOnSystem.value = !isOnSystem.value;
    mqtt.publishMessage(Topics.system, isOnSystem.value.toString());
    underwaterSensorSystem.resetCharts();

    notifyListeners();
  }

  void toggleSampling() {
    isSampling.value = !isSampling.value;
    mqtt.publishMessage(Topics.sampling, isSampling.value.toString());

    if (_timeLeft <= 0) {
      isSampling.value = false;
      _currentTime = 0;
    }

    underwaterSensorSystem.resetCharts();
    notifyListeners();
  }

  //---------------------------SYSTEM TIMER----------------------------------
  int getCurrentTime() {
    return _currentTime;
  }

  void setTimeLeft(int seconds) {
    _timeLeft = seconds;
  }

  //---------------------------UPDATE TIMER-----------------------------------
  void updateTimer(int seconds) {
    _samplingInterval = seconds;
  }

  //---------------------------UPDATE DATA-----------------------------------
  void updateFrontendData(Timer timer) {
    if (!isOnSystem.value) _currentTime = 0;

    for (var i = 0; i < callbacks.length; i++) {
      callbacks.elementAt(i)();
    }
  }
}
