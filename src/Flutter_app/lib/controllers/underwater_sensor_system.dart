import 'dart:async';

import 'package:automatic_deployment_system_app/components/info_card.dart';
import 'package:automatic_deployment_system_app/components/info_graph.dart';
import 'package:automatic_deployment_system_app/controllers/sensor_data.dart';
import 'package:flutter/material.dart';
import 'dart:math' as math;

enum SensorType {
  DEPTH,
  TEMPERATURE,
  PRESSURE,
  BATTERY,
}

typedef UnderwaterSensorSystemCallback = void Function();

class UnderwaterSensorSystemController extends ValueNotifier {
  List<UnderwaterSensorSystemCallback> callbacks = [];

  //DATA
  late SensorDataController _depth;
  late SensorDataController _temperature;
  late SensorDataController _pressure;
  late SensorDataController _battery;

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
  ValueNotifier<bool> isSampling = ValueNotifier(true);

  //WIDGETS
  late List<Infocard> infoCard;
  late List<Infograph> infoGraph;

  UnderwaterSensorSystemController() : super(null);

  void initState() {
    _depth = SensorDataController();
    _temperature = SensorDataController();
    _pressure = SensorDataController();
    _battery = SensorDataController();

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
        axisYMaximum: 5,
        axisYMinimum: -50,
      ),
      Infograph(
        label: "Temperature graph",
        labelAxisy: "Temperature in C",
        sensorData: _temperature,
        axisYMaximum: 30,
        axisYMinimum: -5,
      ),
      Infograph(
        label: "Pressure graph",
        labelAxisy: "Pressure in Pa",
        sensorData: _pressure,
        axisYMaximum: 105000,
        axisYMinimum: 90000,
      ),
      Infograph(
        label: "Battery graph",
        labelAxisy: "Battery life in %",
        sensorData: _battery,
        axisYMaximum: 100,
        axisYMinimum: 0,
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

  //---------------------------RESET SENSORS---------------------------------
  void resetCharts() {
    _depth.resetChartData();
    _temperature.resetChartData();
    _pressure.resetChartData();
    _battery.resetChartData();
  }

  //---------------------------GET SENSORS-----------------------------------
  SensorDataController getDepthSensor() {
    return _depth;
  }

  SensorDataController getTemperatureSensor() {
    return _temperature;
  }

  SensorDataController getPressureSensor() {
    return _pressure;
  }

  SensorDataController getBatterySensor() {
    return _battery;
  }

  //---------------------------GET STATE------------------------------------
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

  int getSamplingInterval() {
    return _samplingInterval;
  }

  //---------------------------GET TOGGLE------------------------------------
  void toggleSystem() {
    isOnSystem.value = !isOnSystem.value;
    resetCharts();

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

    resetCharts();
    notifyListeners();
  }

  //---------------------------GET WIDGETS-----------------------------------

  Infocard getInfocard(int index) {
    return infoCard.elementAt(index);
  }

  Infograph getInfograph(int index) {
    return infoGraph.elementAt(index);
  }

  //---------------------------SYSTEM TIMER----------------------------------
  int getCurrentTime() {
    return _currentTime;
  }

  void setTimeLeft(int seconds) {
    _timeLeft = seconds;
  }

  //---------------------------CALLBACKS-----------------------------------
  void registerCallback(UnderwaterSensorSystemCallback cb) {
    callbacks.add(cb);
  }

  //---------------------------UPDATE DATA-----------------------------------
  void updateData(Timer timer) {
    //Ticks in seconds
    int ticks =
        timer.tick % ((1000 * _samplingInterval) ~/ systemUpdateIntervalMS);

    if (ticks == _samplingInterval && isOnSystem.value && isSampling.value) {
      _currentTime += _samplingInterval;

      updateDepthData();
      updateTemperatureData();
      updatePressureData();
      updateBatteryData();
    }

    if (!isOnSystem.value) _currentTime = 0;

    for (var i = 0; i < callbacks.length; i++) {
      callbacks.elementAt(i)();
    }
  }

  //---------------------------UPDATE TIMER-----------------------------------
  void updateTimer(int seconds) {
    _samplingInterval = seconds;
  }

  //---------------------------UPDATE DEPTH-----------------------------------
  void updateDepthData() {
    double newData = (math.Random().nextDouble() * (-5) - 10);
    _depth.updateCurrentData(newData);

    _depth.sensorData.add(ChartData(_currentTime, _depth.getCurrentData()));

    try {
      _depth.updateGraphData();
    } catch (e) {}
  }

  //---------------------------UPDATE TEMPERATURE-----------------------------
  void updateTemperatureData() {
    double newData = (math.Random().nextDouble() * 5 + 6);
    _temperature.updateCurrentData(newData);

    _temperature.sensorData
        .add(ChartData(_currentTime, _temperature.getCurrentData()));

    try {
      _temperature.updateGraphData();
    } catch (e) {}
  }

  //---------------------------UPDATE PRESSURE--------------------------------
  void updatePressureData() {
    double newData = (math.Random().nextDouble() * 2000 + 100000);
    _pressure.updateCurrentData(newData);

    _pressure.sensorData
        .add(ChartData(_currentTime, _pressure.getCurrentData()));

    try {
      _pressure.updateGraphData();
    } catch (e) {}
  }

  //---------------------------UPDATE BATTERY---------------------------------
  void updateBatteryData() {
    double newData = (math.Random().nextDouble() * 10 + 80);
    _battery.updateCurrentData(newData);

    _battery.sensorData.add(ChartData(_currentTime, _battery.getCurrentData()));

    try {
      _battery.updateGraphData();
    } catch (e) {}
  }
}
