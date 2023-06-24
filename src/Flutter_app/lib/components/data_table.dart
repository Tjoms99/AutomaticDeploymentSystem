import 'package:automatic_deployment_system_app/controllers/USS_controller.dart';
import 'package:automatic_deployment_system_app/controllers/system_controller.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:data_table_2/data_table_2.dart';
import 'package:flutter/material.dart';

class DataSampleTable extends StatefulWidget {
  final SystemController systemController;
  const DataSampleTable({super.key, required this.systemController});

  @override
  State<DataSampleTable> createState() => _DataSampleTableState();
}

class _DataSampleTableState extends State<DataSampleTable> {
  late List<DataRow2> _dataRow;
  late List<double> _depth;
  late List<double> _pressure;
  late List<double> _temperature;
  late List<double> _battery;

  static const List<DataColumn> _dataColumns = [
    DataColumn(label: Text('TIME')),
    DataColumn(label: Text('DEPTH (m)')),
    DataColumn(label: Text('PRESS (Pa)')),
    DataColumn(label: Text('TEMP (°C)')),
    DataColumn(label: Text('BATT (%)')),
  ];

  @override
  void initState() {
    _depth = widget.systemController.underwaterSensorSystem
        .getDepthSensor()
        .getSensorDataList();
    _pressure = widget.systemController.underwaterSensorSystem
        .getPressureSensor()
        .getSensorDataList();
    _temperature = widget.systemController.underwaterSensorSystem
        .getTemperatureSensor()
        .getSensorDataList();
    _battery = widget.systemController.underwaterSensorSystem
        .getBatterySensor()
        .getSensorDataList();

    _dataRow = [];

    super.initState();
  }

  List<DataRow2> _getDataRow() {
    _dataRow = [];
    for (var i = 0; i < _depth.length; i++) {
      try {
        _dataRow.add(
          DataRow2(
            cells: [
              DataCell(Text(
                  '+${i * widget.systemController.getSamplingInterval()} s')),
              DataCell(Text(_depth.elementAt(i).toStringAsFixed(2))),
              DataCell(Text(_pressure.elementAt(i).toStringAsFixed(2))),
              DataCell(Text(_temperature.elementAt(i).toStringAsFixed(2))),
              DataCell(Text(_battery.elementAt(i).toStringAsFixed(2))),
            ],
          ),
        );
      } catch (e) {
        print(
            "Uneven number of elements in array, ingoring the other elements");
      }
    }

    return _dataRow;
  }

  @override
  Widget build(BuildContext context) {
    return ValueListenableBuilder(
      valueListenable: widget.systemController.underwaterSensorSystem
          .getDepthSensor()
          .currentData,
      builder: (context, value, child) {
        return DataTable2(
          minWidth: 500,
          columnSpacing: 0,
          horizontalMargin: 10,
          dividerThickness: 4,
          fixedLeftColumns: 1,
          fixedTopRows: 1,
          columns: _dataColumns,
          rows: _getDataRow(),
          headingTextStyle: const TextStyle(
            color: AppColors.primaryButton,
            fontSize: 17,
            fontWeight: FontWeight.w600,
            fontFamily: 'openSans',
          ),
          dataTextStyle: const TextStyle(color: AppColors.primaryText),
        );
      },
    );
  }
}
