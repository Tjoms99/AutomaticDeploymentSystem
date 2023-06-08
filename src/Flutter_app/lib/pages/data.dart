import 'package:automatic_deployment_system_app/components/data_table.dart';
import 'package:automatic_deployment_system_app/data/underwater_sensor_system.dart';
import 'package:flutter/material.dart';

class DataPage extends StatefulWidget {
  final UnderwaterSensorSystem underwaterSensorSystem;

  const DataPage({super.key, required this.underwaterSensorSystem});

  @override
  State<DataPage> createState() => _DataPageState();
}

class _DataPageState extends State<DataPage> {
  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 10.0, horizontal: 30.0),
      child: DataSampleTable(
          underwaterSensorSystem: widget.underwaterSensorSystem),
    );
  }
}
