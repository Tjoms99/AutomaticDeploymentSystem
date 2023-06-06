import 'package:automatic_deployment_system_app/components/data_table_list.dart';
import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
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
    return DefaultWidget(
      widgets: [
        const Header(label: 'Data', enableUndertext: true),
        SizedBox(height: SizeConfig.blockSizeVertical! * 4),
        DataSampleTableList(
            underwaterSensorSystem: widget.underwaterSensorSystem),
      ],
    );
  }
}