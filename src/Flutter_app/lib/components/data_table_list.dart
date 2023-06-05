import 'package:automatic_deployment_system_app/components/data_table_expand.dart';
import 'package:automatic_deployment_system_app/data/underwater_sensor_system.dart';
import 'package:flutter/material.dart';

class DataSampleTableList extends StatefulWidget {
  final UnderwaterSensorSystem underwaterSensorSystem;
  const DataSampleTableList({super.key, required this.underwaterSensorSystem});

  @override
  State<DataSampleTableList> createState() => _DataSampleTableListState();
}

class _DataSampleTableListState extends State<DataSampleTableList> {
  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.start,
      children: [
        DataSampleTableExpand(
            underwaterSensorSystem: widget.underwaterSensorSystem),
        DataSampleTableExpand(
            underwaterSensorSystem: widget.underwaterSensorSystem),
        DataSampleTableExpand(
            underwaterSensorSystem: widget.underwaterSensorSystem),
      ],
    );
  }
}
