import 'dart:async';
import 'dart:ffi';

import 'package:automatic_deployment_system_app/config/mqtt_topics.dart';
import 'package:automatic_deployment_system_app/controllers/USS_controller.dart';
import 'package:automatic_deployment_system_app/controllers/mqtt_controller.dart';
import 'package:flutter/material.dart';

class SystemController extends ValueNotifier {
  late ValueNotifier<int> currentPage = ValueNotifier(0);
  late USSController underwaterSensorSystem = USSController();
  late MQTTController mqtt = MQTTController();
  double _targetDepth = 6.9;
  int _targetTime = 120;
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

    //MUST BE REGISTERED ACCORDING TO TopicsE INDEXING
    mqtt.registerCallback((data) {
      updateSystem(data);
    });
    mqtt.registerCallback((data) {
      updateSampling(data);
    });
    mqtt.registerCallback((data) {
      updateRS232(data);
    });
    mqtt.registerCallback((data) {
      updateVolt(data);
    });
    mqtt.registerCallback((data) {
      setTargetDepth(double.parse(data));
    });
    mqtt.registerCallback((data) {
      setTargetTime(int.parse(data));
    });
    mqtt.registerCallback((data) {
      setSamplingInterval(int.parse(data));
    });
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

    mqtt.registerCallback((data) {
      updateVolt(data);
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

  void resetDepth() {
    mqtt.publishMessage(Topics.depthInit, '1');
  }

  void resetSystem() {
    //Only used to update MQTT values to sync App and actual state
    isSampling.value = false;
    underwaterSensorSystem.isOnRS232.value = false;
    underwaterSensorSystem.isOn12V.value = false;
    setTargetDepth(0.0);
    setTargetTime(120);
    setSamplingInterval(1);
    mqtt.publishMessage(Topics.sampling, '0');
    mqtt.publishMessage(Topics.rs232, '0');
    mqtt.publishMessage(Topics.volt, '0');
    mqtt.publishMessage(Topics.targetDepth, "0.0");
    mqtt.publishMessage(Topics.targetTime, "0");
    mqtt.publishMessage(Topics.samplingInterval, '1');

    notifyListeners();
    //Reset system
    mqtt.publishMessage(Topics.system, '1');
  }

  //---------------------------TOGGLE-----------------------------------------

  void toggleSampling() {
    isSampling.value = !isSampling.value;

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
    return _currentTime ~/ 10;
  }

  void setTimeLeft(int seconds) {
    _timeLeft = seconds;
  }

  //---------------------------UPDATE DATA-----------------------------------
  void updateFrontendData(Timer timer) {
    _currentTime++; //Updated according to systemUpdateIntervalMS

    if (!isOnSystem.value || !isSampling.value) _currentTime = 0;

    for (var i = 0; i < callbacks.length; i++) {
      callbacks.elementAt(i)();
    }
  }

  void updateSystem(String data) {
    isOnSystem.value = int.parse(data) == 1 ? true : false;
    notifyListeners();
  }

  void updateSampling(String data) {
    isSampling.value = int.parse(data) == 1 ? true : false;
    notifyListeners();
  }

  void updateRS232(String data) {
    underwaterSensorSystem.isOnRS232.value =
        int.parse(data) == 1 ? true : false;
    notifyListeners();
  }

  void updateVolt(String data) {
    underwaterSensorSystem.isOn12V.value = int.parse(data) == 1 ? true : false;
    notifyListeners();
  }
}
