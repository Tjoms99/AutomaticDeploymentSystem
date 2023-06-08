import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:flutter/foundation.dart';

import 'package:syncfusion_flutter_charts/charts.dart';

class SensorData extends ValueNotifier {
  final sensorDataGraphLength = 120;

  ValueNotifier<double> currentData = ValueNotifier(0.0);
  List<double> sensorDataList = [];
  List<ChartData> sensorData = <ChartData>[];
  ChartSeries? series = const ChartSeries();
  ChartSeriesController? chartSeriesController;

  SensorData() : super(null);

  void initState() {
    sensorData = getChartData();
    resetChartData();
  }

  void updateCurrentData(double value) {
    currentData.value = value;
    sensorDataList.add(value);
    notifyListeners();
  }

  double getCurrentData() {
    return currentData.value;
  }

  List<double> getSensorDataList() {
    return sensorDataList;
  }

  List<ChartData> getChartData() {
    return sensorData;
  }

  ChartSeries getDataSeries() {
    return series!;
  }

  void resetChartData() {
    sensorDataList = [];
    sensorData = <ChartData>[];

    //Setup graph series
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

    //Remove all data
    try {
      chartSeriesController?.updateDataSource(removedDataIndexes: [0]);
    } catch (e) {}
  }

  void updateGraphData() {
    if (sensorData.length > sensorDataGraphLength) {
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
