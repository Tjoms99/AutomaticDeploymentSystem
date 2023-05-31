import 'dart:async';

import 'package:automatic_deployment_system_app/components/info_card.dart';
import 'package:automatic_deployment_system_app/components/info_graph.dart';
import 'package:automatic_deployment_system_app/data/sensor_data.dart';
import 'package:flutter/material.dart';
import 'dart:math' as math;

enum SensorType {
  DEPTH,
  TEMPERATURE,
  PRESSURE,
  BATTERY,
}

typedef UnderwaterSensorSystemCallback = void Function();

class UnderwaterSensorSystem extends ValueNotifier {
  List<UnderwaterSensorSystemCallback> callbacks = [];

  //DATA
  late SensorData _depth;
  late SensorData _temperature;
  late SensorData _pressure;
  late SensorData _battery;

  //TIMER
  late Timer _systemTimer;
  int systemUpdateIntervalMS = 100;
  int _currentTime = 0;
  int _timeLeft = 0;

  //SYSTEM FLAGS
  int _samplingInterval = 1;
  ValueNotifier<bool> isOnSystem = ValueNotifier(true);
  ValueNotifier<bool> isOnRS232 = ValueNotifier(false);
  ValueNotifier<bool> isOn12V = ValueNotifier(false);
  ValueNotifier<bool> isSampling = ValueNotifier(false);

  //WIDGETS
  late List<Infocard> infoCard;
  late List<Infograph> infoGraph;

  UnderwaterSensorSystem() : super(null);

  void initState() {
    _depth = SensorData();
    _temperature = SensorData();
    _pressure = SensorData();
    _battery = SensorData();

    _systemTimer = Timer.periodic(
        Duration(milliseconds: systemUpdateIntervalMS), updateData);

    _depth.initState();
    _temperature.initState();
    _pressure.initState();
    _battery.initState();

    infoGraph = [
      Infograph(
        label: "Depth graph",
        labelAxisy: "Depth in meters",
        sensorData: _depth,
        AxisYMaximum: 5,
        AxisYMinimum: -50,
      ),
      Infograph(
        label: "Temperature graph",
        labelAxisy: "Temperature in C",
        sensorData: _temperature,
        AxisYMaximum: 30,
        AxisYMinimum: -5,
      ),
      Infograph(
        label: "Pressure graph",
        labelAxisy: "Pressure in Pa",
        sensorData: _pressure,
        AxisYMaximum: 105000,
        AxisYMinimum: 90000,
      ),
      Infograph(
        label: "Battery graph",
        labelAxisy: "Battery life in %",
        sensorData: _battery,
        AxisYMaximum: 100,
        AxisYMinimum: 0,
      ),
    ];

    infoCard = [
      Infocard(
        icon: Icons.waves,
        label: "Depth",
        sensorData: _depth,
      ),
      Infocard(
        icon: Icons.thermostat,
        label: "Temperature",
        sensorData: _temperature,
      ),
      Infocard(
        icon: Icons.vertical_align_center,
        label: "Pressure",
        sensorData: _pressure,
      ),
      Infocard(
        icon: Icons.battery_std,
        label: "Battery",
        sensorData: _battery,
      )
    ];
  }

  //----------SENSORS---------------
  SensorData getDepthSensor() {
    return _depth;
  }

  SensorData getTemperatureSensor() {
    return _temperature;
  }

  SensorData getPressureSensor() {
    return _pressure;
  }

  SensorData getBatterySensor() {
    return _battery;
  }

  //----------SYSTEM---------------
  bool getIsOnSystem() {
    return isOnSystem.value;
  }

  bool getIsOnRS232() {
    return isOnRS232.value;
  }

  bool getIsOn12V() {
    return isOn12V.value;
  }

  bool getIsSampling() {
    if (_timeLeft <= 0) {
      isSampling.value = false;
      _currentTime = 0;
    }

    return isSampling.value;
  }

  void toggleSystem() {
    isOnSystem.value = !isOnSystem.value;
    notifyListeners();
  }

  void toggleRS232() {
    isOnRS232.value = !isOnRS232.value;
    notifyListeners();
  }

  void toggle12V() {
    isOn12V.value = !isOn12V.value;
    notifyListeners();
  }

  void toggleSampling() {
    isSampling.value = !isSampling.value;
    if (_timeLeft <= 0) {
      isSampling.value = false;
      _currentTime = 0;
    }
    notifyListeners();
  }

  void setTimeLeft(int seconds) {
    _timeLeft = seconds;
  }

  Infocard getInfocard(int index) {
    return infoCard.elementAt(index);
  }

  Infograph getInfograph(int index) {
    return infoGraph.elementAt(index);
  }

  int getCurrentTime() {
    return _currentTime;
  }

  void registerCallback(UnderwaterSensorSystemCallback cb) {
    callbacks.add(cb);
  }

  //----------UPDATE---------------
  void updateData(Timer timer) {
    //Ticks in seconds
    int ticks = timer.tick % (1000 ~/ systemUpdateIntervalMS);

    if (ticks == 0 && isOnSystem.value && isSampling.value) {
      _currentTime += _samplingInterval;

      updateDepthData();
      updateTemperatureData();
      updatePressureData();
      updateBatteryData();
    }

    //Restart widgets ......
    if (!isOnSystem.value) _currentTime = 0;

    for (var i = 0; i < callbacks.length; i++) {
      callbacks.elementAt(i)();
    }
  }

  void updateTimer(int seconds) {
    _samplingInterval = seconds;
    // _systemTimer.cancel();
    //  _systemTimer =
    //    Timer.periodic(Duration(seconds: _samplingInterval), updateData);
  }

  void updateDepthData() {
    double newData = (math.Random().nextDouble() * (-5) - 10);
    _depth.updateCurrentData(newData);

    _depth.sensorData.add(ChartData(_currentTime, _depth.getCurrentData()));

    try {
      _depth.updateGraphData();
    } catch (e) {}
  }

  void updateTemperatureData() {
    double newData = (math.Random().nextDouble() * 5 + 6);
    _temperature.updateCurrentData(newData);

    _temperature.sensorData
        .add(ChartData(_currentTime, _temperature.getCurrentData()));

    try {
      _temperature.updateGraphData();
    } catch (e) {}
  }

  void updatePressureData() {
    double newData = (math.Random().nextDouble() * 2000 + 100000);
    _pressure.updateCurrentData(newData);

    _pressure.sensorData
        .add(ChartData(_currentTime, _pressure.getCurrentData()));

    try {
      _pressure.updateGraphData();
    } catch (e) {}
  }

  void updateBatteryData() {
    double newData = (math.Random().nextDouble() * 10 + 80);
    _battery.updateCurrentData(newData);

    _battery.sensorData.add(ChartData(_currentTime, _battery.getCurrentData()));

    try {
      _battery.updateGraphData();
    } catch (e) {}
  }
}
