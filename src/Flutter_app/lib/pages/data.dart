import 'package:automatic_deployment_system_app/components/data_table.dart';
import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/data/underwater_sensor_system.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:flutter/material.dart';
import 'package:flutter_expandable_table/flutter_expandable_table.dart';

class DataPage extends StatefulWidget {
  UnderwaterSensorSystem underwaterSensorSystem;

  DataPage({super.key, required this.underwaterSensorSystem});

  @override
  State<DataPage> createState() => _DataPageState();
}

class _DataPageState extends State<DataPage> {
  @override
  Widget build(BuildContext context) {
    return DefaultWidget(
      widgets: [
        Header(label: 'Data', enableUndertext: true),
        SizedBox(height: SizeConfig.blockSizeVertical!),
        DataSampleTable(underwaterSensorSystem: widget.underwaterSensorSystem),
      ],
    );
  }
}
