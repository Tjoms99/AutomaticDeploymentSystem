import 'package:automatic_deployment_system_app/components/control_card.dart';
import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/controllers/mqtt_controller.dart';
import 'package:automatic_deployment_system_app/controllers/USS_controller.dart';
import 'package:automatic_deployment_system_app/controllers/system_controller.dart';
import 'package:flutter/material.dart';

class ControlPage extends StatefulWidget {
  final SystemController systemController;
  const ControlPage({super.key, required this.systemController});

  @override
  State<ControlPage> createState() => _ControlPageState();
}

class _ControlPageState extends State<ControlPage> {
  @override
  Widget build(BuildContext context) {
    return DefaultWidget(
      widgets: [
        const Header(label: 'Control', enableUndertext: true),
        SizedBox(height: SizeConfig.blockSizeVertical! * 4),
        Wrap(
          runSpacing: 20.0,
          spacing: 20.0,
          alignment: WrapAlignment.start,
          children: [
            Controlcard(
              label: 'SYSTEM',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.systemController.isOnSystem,
              callback: widget.systemController.toggleSystem,
              getStatus: widget.systemController.getIsOnSystem,
            ),
            Controlcard(
              label: 'SAMPLING',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.systemController.isSampling,
              callback: widget.systemController.toggleSampling,
              getStatus: widget.systemController.getIsSampling,
            ),
            /*
            Controlcard(
              label: 'RS232',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.mqttController.USS.isOnRS232,
              callback: widget.mqttController.toggleRS232,
              getStatus: widget.mqttController.USS.getIsOnRS232,
            ),
            Controlcard(
              label: '12V',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.mqttController.USS.isOn12V,
              callback: widget.mqttController.toggle12V,
              getStatus: widget.mqttController.USS.getIsOn12V,
            ),
            
            Controlcard(
              label: 'MODE STATUS',
              textEnable: "AUTOMATIC",
              textDisable: "MANUAL",
              enabled: true,
            ),
            Controlcard(
              label: 'MANUAL',
              textEnable: "STARTED",
              textDisable: "STOPPED",
              enabled: true,
            ),
            Controlcard(
              label: 'REEL',
              textEnable: "REELING",
              textDisable: "NOP",
              enabled: true,
            ),
            Controlcard(
              label: 'RELEASE',
              textEnable: "RELEASING",
              textDisable: "NOP",
              enabled: true,
            ),

            //Should only be triggers....
            Controlcard(
              label: 'RESET',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: true,
            ),
            Controlcard(
              label: 'UPDATE STATE',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: true,
            ),
            */
          ],
        ),
      ],
    );
  }
}
