import 'package:automatic_deployment_system_app/components/info_card.dart';
import 'package:automatic_deployment_system_app/components/info_graph.dart';
import 'package:automatic_deployment_system_app/components/sidemenu_info.dart';
import 'package:automatic_deployment_system_app/data/sensor_data.dart';
import 'package:automatic_deployment_system_app/data/sensor_data_list.dart';
import 'package:automatic_deployment_system_app/pages/control.dart';
import 'package:automatic_deployment_system_app/pages/dashboard.dart';
import 'package:automatic_deployment_system_app/components/sidemenu_buttons.dart';
import 'package:automatic_deployment_system_app/data/graphs.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

class ParentPage extends StatefulWidget {
  const ParentPage({super.key});

  @override
  State<ParentPage> createState() => _ParentPageState();
}

class _ParentPageState extends State<ParentPage> {
  ValueNotifier<int> currentPage = ValueNotifier(0);
  SensorDataList sensorDataList = SensorDataList();

  @override
  void initState() {
    sensorDataList.initState();
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    SystemChrome.setEnabledSystemUIMode(SystemUiMode.immersiveSticky);
    return Scaffold(
      resizeToAvoidBottomInset: false,
      body: GestureDetector(
        onTap: () => FocusManager.instance.primaryFocus?.unfocus(),
        child: SafeArea(
            bottom: false,
            left: false,
            right: false,
            top: true,
            child: Row(
              children: [
                Expanded(
                  flex: 1,
                  child: SideMenuButtons(currentPage: currentPage),
                ),
                Expanded(
                  flex: 10,
                  child: ValueListenableBuilder(
                    valueListenable: currentPage,
                    builder: (context, value, child) {
                      return value == 0
                          ? Dashboard(sensorDataList: sensorDataList)
                          : const ControlPage();
                    },
                  ),
                ),
                Expanded(
                  flex: 4,
                  child: SideMenuInfo(sensorDataList: sensorDataList),
                ),
              ],
            )),
      ),
    );
  }
}
