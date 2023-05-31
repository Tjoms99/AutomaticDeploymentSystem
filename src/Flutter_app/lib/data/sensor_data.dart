import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:flutter/foundation.dart';

import 'package:syncfusion_flutter_charts/charts.dart';

class SensorData extends ValueNotifier {
  ValueNotifier<double> currentData = ValueNotifier(0.0);

  List<ChartData> sensorData = <ChartData>[];
  ChartSeries? series = const ChartSeries();
  ChartSeriesController? chartSeriesController;

  SensorData() : super(null);

  void updateCurrentData(double value) {
    currentData.value = value;
    notifyListeners();
  }

  double getCurrentData() {
    return currentData.value;
  }

  List<ChartData> getChartData() {
    return sensorData;
  }

  void resetChartData() {
    sensorData = <ChartData>[];
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

    chartSeriesController?.updateDataSource(removedDataIndexes: [
      0,
      1,
      2,
      3,
      4,
      5,
      6,
      7,
      8,
      9,
      10,
      11,
      12,
      13,
      14,
      15,
      16,
      17,
      18,
      19
    ]);
  }

  ChartSeries getDataSeries() {
    return series!;
  }

  void initState() {
    sensorData = getChartData();
    resetChartData();
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
