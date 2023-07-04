import 'package:automatic_deployment_system_app/components/info_card.dart';
import 'package:automatic_deployment_system_app/components/info_graph.dart';
import 'package:automatic_deployment_system_app/controllers/sensor_controller.dart';
import 'package:flutter/material.dart';

enum SensorType {
  DEPTH,
  TEMPERATURE,
  PRESSURE,
  BATTERY,
}

class USSController extends ValueNotifier {
  //DATA
  late SensorDataController _depth;
  late SensorDataController _temperature;
  late SensorDataController _pressure;
  late SensorDataController _battery;

  //SYSTEM FLAGS
  ValueNotifier<bool> isOnRS232 = ValueNotifier(false);
  ValueNotifier<bool> isOn12V = ValueNotifier(false);

  //WIDGETS
  late List<Infocard> infoCard;
  late List<Infograph> infoGraph;

  USSController() : super(null);

  void initState() {
    _depth = SensorDataController();
    _temperature = SensorDataController();
    _pressure = SensorDataController();
    _battery = SensorDataController();

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
        axisYMaximum: 3100,
        axisYMinimum: 900,
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

  bool getIsOnRS232() {
    return isOnRS232.value;
  }

  bool getIsOn12V() {
    return isOn12V.value;
  }

  //---------------------------TOGGLE-----------------------------------------

  void toggleRS232() {
    isOnRS232.value = !isOnRS232.value;
    notifyListeners();
  }

  void toggle12V() {
    isOn12V.value = !isOn12V.value;
    notifyListeners();
  }

  //---------------------------GET WIDGETS-----------------------------------

  Infocard getInfocard(int index) {
    return infoCard.elementAt(index);
  }

  Infograph getInfograph(int index) {
    return infoGraph.elementAt(index);
  }

  //---------------------------UPDATE DEPTH-----------------------------------
  void updateDepth(String data) {
    _depth.updateCurrentData(double.parse(data));

    int samplingInterval = 1;
    int currentTime = samplingInterval * _depth.sensorData.length;

    _depth.sensorData.add(ChartData(currentTime, _depth.getCurrentData()));

    try {
      _depth.updateGraphData();
    } catch (e) {}
  }

  //---------------------------UPDATE TEMPERATURE-----------------------------
  void updateTemperature(String data) {
    _temperature.updateCurrentData(double.parse(data));

    int samplingInterval = 1;
    int currentTime = samplingInterval * _depth.sensorData.length;

    _temperature.sensorData
        .add(ChartData(currentTime, _temperature.getCurrentData()));

    try {
      _temperature.updateGraphData();
    } catch (e) {}
  }

  //---------------------------UPDATE PRESSURE--------------------------------
  void updatePressure(String data) {
    _pressure.updateCurrentData(double.parse(data) / 100); // From Pa to mBar

    int samplingInterval = 1;
    int currentTime = samplingInterval * _depth.sensorData.length;

    _pressure.sensorData
        .add(ChartData(currentTime, _pressure.getCurrentData()));

    try {
      _pressure.updateGraphData();
    } catch (e) {}
  }

  //---------------------------UPDATE BATTERY---------------------------------
  void updateBattery(String data) {
    _battery.updateCurrentData(double.parse(data));

    int samplingInterval = 1;
    int currentTime = samplingInterval * _depth.sensorData.length;

    _battery.sensorData.add(ChartData(currentTime, _battery.getCurrentData()));

    try {
      _battery.updateGraphData();
    } catch (e) {}
  }
}
