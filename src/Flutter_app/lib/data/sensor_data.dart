import 'dart:async';

import 'package:automatic_deployment_system_app/data/graphs.dart';
import 'package:automatic_deployment_system_app/data/sensor_data_list.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:flutter/foundation.dart';

import 'package:syncfusion_flutter_charts/charts.dart';

class SensorData extends ValueNotifier {
  List<SensorDataCallback> callback = [];
  ValueNotifier<int> currentTime = ValueNotifier(0);
  ValueNotifier<double> currentData = ValueNotifier(0.0);

  List<ChartData> sensorData = <ChartData>[];
  ChartSeries? series = const ChartSeries();
  ChartSeriesController? chartSeriesController;

  SensorData() : super(null);

  void registerCallback(SensorDataCallback cb) {
    callback.add(cb);
  }

  void updateCurrentTime(int value) {
    currentTime.value = value;
    notifyListeners();
  }

  void updateCurrentData(double value) {
    currentData.value = value;
    notifyListeners();
  }

  int getCurrentTime() {
    return currentTime.value;
  }

  double getCurrentData() {
    return currentData.value;
  }

  List<ChartData> getChartData() {
    return sensorData;
  }

  ChartSeries getDataSeries() {
    return series!;
  }

  void initState() {
    sensorData = getChartData();
    Timer.periodic(const Duration(seconds: 1), update);

    series = SplineSeries<ChartData, int>(
      onRendererCreated: (ChartSeriesController controller) {
        chartSeriesController = controller;
      },
      color: AppColors.primaryBackground,
      width: 4,
      dataSource: getChartData(),
      dataLabelSettings: const DataLabelSettings(
          isVisible: false,
          color: AppColors.primaryBackground,
          labelPosition: ChartDataLabelPosition.outside),
      xValueMapper: (ChartData data, _) => data.x,
      yValueMapper: (ChartData data, _) =>
          double.parse(data.y.toStringAsFixed(2)),
    );
  }

  //TODO: Needs to be a callback function that is passed to the widget
  //updateDepth, updateTemperature, update... ect.
  void update(Timer timer) {
    int callbackLength = callback.length;
    for (var i = 0; i < callbackLength; i++) {
      callback.elementAt(i)();
    }
  }

  void updateGraphData() {
    //Displays max 20 elements
    if (sensorData.length > 20) {
      sensorData.removeAt(0);
      chartSeriesController?.updateDataSource(
          addedDataIndex: sensorData.length - 1, removedDataIndex: 0);
    } else {
      chartSeriesController?.updateDataSource(
          addedDataIndex: sensorData.length - 1);
    }
  }
}

class ChartData {
  ChartData(this.x, this.y);

  final int x;
  final double y;
}
