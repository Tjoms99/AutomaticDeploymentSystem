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

class UnderwaterSensorSystem {
  List<UnderwaterSensorSystemCallback> callbacks = [];

  //DATA
  late SensorData _depth;
  late SensorData _temperature;
  late SensorData _pressure;
  late SensorData _battery;

  //TIMER
  late Timer _systemTimer;
  int currentTime = 0;

  //SYSTEM FLAGS
  int samplingInterval = 1;

  //WIDGETS
  late List<Infocard> infoCard;
  late List<Infograph> infoGraph;

  UnderwaterSensorSystem();

  void initState() {
    _depth = SensorData();
    _temperature = SensorData();
    _pressure = SensorData();
    _battery = SensorData();

    _systemTimer =
        Timer.periodic(Duration(seconds: samplingInterval), updateData);

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

  Infocard getInfocard(int index) {
    return infoCard.elementAt(index);
  }

  Infograph getInfograph(int index) {
    return infoGraph.elementAt(index);
  }

  void registerCallback(UnderwaterSensorSystemCallback cb) {
    callbacks.add(cb);
  }

  void updateData(Timer timer) {
    currentTime += samplingInterval;

    updateDepthData();
    updateTemperatureData();
    updatePressureData();
    updateBatteryData();

    for (var i = 0; i < callbacks.length; i++) {
      callbacks.elementAt(i)();
    }
  }

  void updateTimer(int seconds) {
    samplingInterval = seconds;
    _systemTimer.cancel();
    _systemTimer =
        Timer.periodic(Duration(seconds: samplingInterval), updateData);
  }

  void updateDepthData() {
    double newData = (math.Random().nextDouble() * (-5) - 10);
    _depth.updateCurrentData(newData);

    _depth.sensorData.add(ChartData(currentTime, _depth.getCurrentData()));

    try {
      _depth.updateGraphData();
    } catch (e) {}
  }

  void updateTemperatureData() {
    double newData = (math.Random().nextDouble() * 5 + 6);
    _temperature.updateCurrentData(newData);

    _temperature.sensorData
        .add(ChartData(currentTime, _temperature.getCurrentData()));

    try {
      _temperature.updateGraphData();
    } catch (e) {}
  }

  void updatePressureData() {
    double newData = (math.Random().nextDouble() * 2000 + 100000);
    _pressure.updateCurrentData(newData);

    _pressure.sensorData
        .add(ChartData(currentTime, _pressure.getCurrentData()));

    try {
      _pressure.updateGraphData();
    } catch (e) {}
  }

  void updateBatteryData() {
    double newData = (math.Random().nextDouble() * 10 + 80);
    _battery.updateCurrentData(newData);

    _battery.sensorData.add(ChartData(currentTime, _battery.getCurrentData()));

    try {
      _battery.updateGraphData();
    } catch (e) {}
  }
}
