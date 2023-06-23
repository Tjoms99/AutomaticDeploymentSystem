import 'package:automatic_deployment_system_app/components/sidemenu_info.dart';
import 'package:automatic_deployment_system_app/controllers/mqtt_controller.dart';
import 'package:automatic_deployment_system_app/controllers/underwater_sensor_system.dart';
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
  late ValueNotifier<int> currentPage;
  late UnderwaterSensorSystemController underwaterSensorSystem;
  late MQTTController mqttController;

  late List<Widget> mainPages;

  @override
  void initState() {
    underwaterSensorSystem = UnderwaterSensorSystemController();
    underwaterSensorSystem.initState();

    mqttController = MQTTController(underwaterSensorSystem);
    mqttController.connect();

    currentPage = ValueNotifier(0);

    mainPages = [
      Dashboard(underwaterSensorSystem: underwaterSensorSystem),
      ControlPage(mqttController: mqttController),
      DataPage(underwaterSensorSystem: underwaterSensorSystem),
    ];

    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    //Set fullscreen for phones
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
