import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/components/info_card.dart';
import 'package:automatic_deployment_system_app/components/info_graph.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/data/graphs.dart';
import 'package:automatic_deployment_system_app/data/sensor_data_list.dart';
import 'package:flutter/material.dart';

class Dashboard extends StatefulWidget {
  SensorDataList sensorDataList;

  Dashboard({super.key, required this.sensorDataList});

  @override
  State<Dashboard> createState() => _DashboardState();
}

class _DashboardState extends State<Dashboard> {
  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return DefaultWidget(widgets: [
      Header(label: 'Dashboard', enableUndertext: true),
      SizedBox(height: SizeConfig.blockSizeVertical! * 4),
      Wrap(
        runSpacing: 20.0,
        spacing: 20.0,
        alignment: WrapAlignment.start,
        children: [
          widget.sensorDataList.getInfocard(SensorType.DEPTH.index),
          widget.sensorDataList.getInfocard(SensorType.TEMPERATURE.index),
          widget.sensorDataList.getInfocard(SensorType.PRESSURE.index),
          widget.sensorDataList.getInfocard(SensorType.BATTERY.index),
        ],
      ),
      SizedBox(height: SizeConfig.blockSizeVertical! * 4),
      widget.sensorDataList.getInfograph(SensorType.DEPTH.index),
      widget.sensorDataList.getInfograph(SensorType.TEMPERATURE.index),
      widget.sensorDataList.getInfograph(SensorType.PRESSURE.index),
      widget.sensorDataList.getInfograph(SensorType.BATTERY.index),
    ]);
  }
}
