import 'package:automatic_deployment_system_app/data/underwater_sensor_system.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:flutter/material.dart';

class DataSampleTable extends StatefulWidget {
  final UnderwaterSensorSystem underwaterSensorSystem;
  const DataSampleTable({super.key, required this.underwaterSensorSystem});

  @override
  State<DataSampleTable> createState() => _DataSampleTableState();
}

class _DataSampleTableState extends State<DataSampleTable> {
  late List<DataRow> _dataRow;
  late List<double> _depth;
  late List<double> _pressure;
  late List<double> _temperature;
  late List<double> _battery;
  static const List<DataColumn> _dataColumn = [
    DataColumn(
      label: Text('TIME'),
    ),
    DataColumn(
      label: Text('DEPTH'),
    ),
    DataColumn(
      label: Text('PRESSURE'),
    ),
    DataColumn(
      label: Text('TEMPERATURE'),
    ),
    DataColumn(
      label: Text('BATTERY'),
    ),
  ];

  @override
  void initState() {
    _depth = widget.underwaterSensorSystem.getDepthSensor().getSensorDataList();
    _pressure =
        widget.underwaterSensorSystem.getPressureSensor().getSensorDataList();
    _temperature = widget.underwaterSensorSystem
        .getTemperatureSensor()
        .getSensorDataList();
    _battery =
        widget.underwaterSensorSystem.getBatterySensor().getSensorDataList();

    _dataRow = [];

    super.initState();
  }

  List<DataRow> _getDataRow() {
    _dataRow = [];

    for (var i = 0; i < _depth.length; i++) {
      _dataRow.add(
        DataRow(
          cells: [
            DataCell(Text(i.toStringAsFixed(2))),
            DataCell(Text(_depth.elementAt(i).toStringAsFixed(2))),
            DataCell(Text(_pressure.elementAt(i).toStringAsFixed(2))),
            DataCell(Text(_temperature.elementAt(i).toStringAsFixed(2))),
            DataCell(Text(_battery.elementAt(i).toStringAsFixed(2))),
          ],
        ),
      );
    }

    return _dataRow;
  }

  @override
  Widget build(BuildContext context) {
    return SingleChildScrollView(
      scrollDirection: Axis.horizontal,
      child: ValueListenableBuilder(
        valueListenable:
            widget.underwaterSensorSystem.getDepthSensor().currentData,
        builder: (context, value, child) {
          return DataTable(
            columns: _dataColumn,
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
      ),
    );
  }
}
