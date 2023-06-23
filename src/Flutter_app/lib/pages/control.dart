import 'package:automatic_deployment_system_app/components/control_card.dart';
import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/controllers/mqtt_controller.dart';
import 'package:automatic_deployment_system_app/controllers/underwater_sensor_system.dart';
import 'package:flutter/material.dart';

class ControlPage extends StatefulWidget {
  final MQTTController mqttController;
  const ControlPage({super.key, required this.mqttController});

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
              enabled: widget.mqttController.underwaterSensorSystem.isOnSystem,
              callback: widget.mqttController.toggleSystem,
              getStatus:
                  widget.mqttController.underwaterSensorSystem.getIsOnSystem,
            ),
            Controlcard(
              label: 'SAMPLING',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.mqttController.underwaterSensorSystem.isSampling,
              callback: widget.mqttController.toggleSampling,
              getStatus:
                  widget.mqttController.underwaterSensorSystem.getIsSampling,
            ),
            Controlcard(
              label: 'RS232',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.mqttController.underwaterSensorSystem.isOnRS232,
              callback: widget.mqttController.toggleRS232,
              getStatus:
                  widget.mqttController.underwaterSensorSystem.getIsOnRS232,
            ),
            Controlcard(
              label: '12V',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.mqttController.underwaterSensorSystem.isOn12V,
              callback: widget.mqttController.toggle12V,
              getStatus:
                  widget.mqttController.underwaterSensorSystem.getIsOn12V,
            ),
            /*
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
