import 'package:automatic_deployment_system_app/components/dashboard_contents.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/components/sidemenu.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/data/graphs.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

class ParentPage extends StatefulWidget {
  ParentPage({super.key});

  @override
  State<ParentPage> createState() => _ParentPageState();
}

class _ParentPageState extends State<ParentPage> {
  ValueNotifier<int> currentPage = ValueNotifier(0);

  @override
  void initState() {
    graphList.elementAt(SensorType.DEPTH.index).sensorData.initState();
    graphList.elementAt(SensorType.TEMPERATURE.index).sensorData.initState();
    graphList.elementAt(SensorType.PRESSURE.index).sensorData.initState();
    graphList.elementAt(SensorType.BATTERY.index).sensorData.initState();

    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    SystemChrome.setEnabledSystemUIMode(SystemUiMode.immersiveSticky);
    return Scaffold(
      resizeToAvoidBottomInset: false,
      body: SafeArea(
          bottom: false,
          left: false,
          right: false,
          top: true,
          child: Row(
            children: [
              Expanded(
                flex: 1,
                child: SideMenu(currentPage: currentPage),
              ),
              Expanded(
                flex: 10,
                child: ValueListenableBuilder(
                  valueListenable: currentPage,
                  builder: (context, value, child) {
                    return value == 0 ? DashboardContent() : SizedBox();
                  },
                ),
              ),
              Expanded(
                flex: 4,
                child: SideMenu(currentPage: currentPage),
              ),
            ],
          )),
    );
  }
}
