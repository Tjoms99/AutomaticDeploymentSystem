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

typedef SensorDataCallback = void Function();

SensorData _depth = SensorData(callback: updateDepthData);
SensorData _temperature = SensorData(callback: updateTemperatureData);
SensorData _pressure = SensorData(callback: updatePressureData);
SensorData _battery = SensorData(callback: updateBatteryData);

void updateDepthData() {
  double newData = (math.Random().nextDouble() * (-5) - 10);
  _depth.updateCurrentData(newData);

  int newTime = (_depth.getCurrentTime() + 1);
  _depth.updateCurrentTime(newTime);
  _depth.sensorData
      .add(ChartData(_depth.getCurrentTime(), _depth.getCurrentData()));

  //Update last
  try {
    _depth.updateGraphData();
  } catch (e) {}
}

void updateTemperatureData() {
  double newData = (math.Random().nextDouble() * 5 + 6);
  _temperature.updateCurrentData(newData);

  int newTime = (_temperature.getCurrentTime() + 1);
  _temperature.updateCurrentTime(newTime);
  _temperature.sensorData.add(
      ChartData(_temperature.getCurrentTime(), _temperature.getCurrentData()));
  //Update last
  try {
    _temperature.updateGraphData();
  } catch (e) {}
}

void updatePressureData() {
  double newData = (math.Random().nextDouble() * 2000 + 100000);
  _pressure.updateCurrentData(newData);

  int newTime = (_pressure.getCurrentTime() + 1);
  _pressure.updateCurrentTime(newTime);
  _pressure.sensorData
      .add(ChartData(_pressure.getCurrentTime(), _pressure.getCurrentData()));
  //Update last
  try {
    _pressure.updateGraphData();
  } catch (e) {}
}

void updateBatteryData() {
  double newData = (math.Random().nextDouble() * 10 + 80);
  _battery.updateCurrentData(newData);

  int newTime = (_battery.getCurrentTime() + 1);
  _battery.updateCurrentTime(newTime);
  _battery.sensorData
      .add(ChartData(_battery.getCurrentTime(), _battery.getCurrentData()));
  //Update last
  try {
    _battery.updateGraphData();
  } catch (e) {}
}

List<Infograph> graphList = [
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

List<Infocard> cardList = [
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