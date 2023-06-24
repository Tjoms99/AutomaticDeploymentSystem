import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/controllers/USS_controller.dart';
import 'package:flutter/material.dart';

class Dashboard extends StatefulWidget {
  final USSController USS;

  const Dashboard({super.key, required this.USS});

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
          widget.USS.getInfocard(SensorType.DEPTH.index),
          widget.USS.getInfocard(SensorType.TEMPERATURE.index),
          widget.USS.getInfocard(SensorType.PRESSURE.index),
          widget.USS.getInfocard(SensorType.BATTERY.index),
        ],
      ),
      SizedBox(height: SizeConfig.blockSizeVertical! * 4),
      widget.USS.getInfograph(SensorType.DEPTH.index),
      widget.USS.getInfograph(SensorType.TEMPERATURE.index),
      widget.USS.getInfograph(SensorType.PRESSURE.index),
      widget.USS.getInfograph(SensorType.BATTERY.index),
    ]);
  }
}
