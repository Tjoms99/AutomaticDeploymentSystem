import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/data/graphs.dart';
import 'package:flutter/material.dart';

class Dashboard extends StatefulWidget {
  const Dashboard({super.key});

  @override
  State<Dashboard> createState() => _DashboardState();
}

class _DashboardState extends State<Dashboard> {
  @override
  Widget build(BuildContext context) {
    return DefaultWidget(widgets: [
      Header(label: 'Dashboard'),
      SizedBox(height: SizeConfig.blockSizeVertical! * 4),
      Wrap(
        runSpacing: 20.0,
        spacing: 20.0,
        alignment: WrapAlignment.start,
        children: [
          cardList.elementAt(SensorType.DEPTH.index),
          cardList.elementAt(SensorType.TEMPERATURE.index),
          cardList.elementAt(SensorType.PRESSURE.index),
          cardList.elementAt(SensorType.BATTERY.index),
        ],
      ),
      SizedBox(height: SizeConfig.blockSizeVertical! * 4),
      graphList.elementAt(SensorType.DEPTH.index),
      graphList.elementAt(SensorType.TEMPERATURE.index),
      graphList.elementAt(SensorType.PRESSURE.index),
      graphList.elementAt(SensorType.BATTERY.index),
    ]);
  }
}
