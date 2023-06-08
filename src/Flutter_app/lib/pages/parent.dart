import 'package:automatic_deployment_system_app/components/sidemenu_info.dart';
import 'package:automatic_deployment_system_app/data/underwater_sensor_system.dart';
import 'package:automatic_deployment_system_app/pages/control.dart';
import 'package:automatic_deployment_system_app/pages/dashboard.dart';
import 'package:automatic_deployment_system_app/components/sidemenu_buttons.dart';
import 'package:automatic_deployment_system_app/pages/data.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

class ParentPage extends StatefulWidget {
  const ParentPage({super.key});

  @override
  State<ParentPage> createState() => _ParentPageState();
}

class _ParentPageState extends State<ParentPage> {
  ValueNotifier<int> currentPage = ValueNotifier(0);
  UnderwaterSensorSystem underwaterSensorSystem = UnderwaterSensorSystem();
  late List<Widget> mainPages;

  @override
  void initState() {
    underwaterSensorSystem.initState();
    mainPages = [
      Dashboard(underwaterSensorSystem: underwaterSensorSystem),
      ControlPage(underwaterSensorSystem: underwaterSensorSystem),
      DataPage(underwaterSensorSystem: underwaterSensorSystem),
    ];
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
                      return mainPages[value];
                    },
                  ),
                ),
                Expanded(
                  flex: 4,
                  child: SideMenuInfo(
                      underwaterSensorSytem: underwaterSensorSystem),
                ),
              ],
            )),
      ),
    );
  }
}
