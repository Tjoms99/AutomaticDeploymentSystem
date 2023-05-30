import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/data/underwater_sensor_system.dart';
import 'package:flutter/material.dart';

class Dashboard extends StatefulWidget {
  final UnderwaterSensorSystem underwaterSensorSystem;

  const Dashboard({super.key, required this.underwaterSensorSystem});

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
      const Header(label: 'Dashboard', enableUndertext: true),
      SizedBox(height: SizeConfig.blockSizeVertical! * 4),
      Wrap(
        runSpacing: 20.0,
        spacing: 20.0,
        alignment: WrapAlignment.start,
        children: [
          widget.underwaterSensorSystem.getInfocard(SensorType.DEPTH.index),
          widget.underwaterSensorSystem
              .getInfocard(SensorType.TEMPERATURE.index),
          widget.underwaterSensorSystem.getInfocard(SensorType.PRESSURE.index),
          widget.underwaterSensorSystem.getInfocard(SensorType.BATTERY.index),
        ],
      ),
      SizedBox(height: SizeConfig.blockSizeVertical! * 4),
      widget.underwaterSensorSystem.getInfograph(SensorType.DEPTH.index),
      widget.underwaterSensorSystem.getInfograph(SensorType.TEMPERATURE.index),
      widget.underwaterSensorSystem.getInfograph(SensorType.PRESSURE.index),
      widget.underwaterSensorSystem.getInfograph(SensorType.BATTERY.index),
    ]);
  }
}
