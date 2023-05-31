import 'package:automatic_deployment_system_app/components/control_card.dart';
import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/data/underwater_sensor_system.dart';
import 'package:flutter/material.dart';

class ControlPage extends StatefulWidget {
  final UnderwaterSensorSystem underwaterSensorSystem;
  const ControlPage({required this.underwaterSensorSystem});

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
              enabled: widget.underwaterSensorSystem.isOnSystem,
              callback: widget.underwaterSensorSystem.toggleSystem,
              getStatus: widget.underwaterSensorSystem.getIsOnSystem,
            ),
            Controlcard(
              label: 'SAMPLING',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.underwaterSensorSystem.isSampling,
              callback: widget.underwaterSensorSystem.toggleSampling,
              getStatus: widget.underwaterSensorSystem.getIsSampling,
            ),
            Controlcard(
              label: 'RS232',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.underwaterSensorSystem.isOnRS232,
              callback: widget.underwaterSensorSystem.toggleRS232,
              getStatus: widget.underwaterSensorSystem.getIsOnRS232,
            ),
            Controlcard(
              label: '12V',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.underwaterSensorSystem.isOn12V,
              callback: widget.underwaterSensorSystem.toggle12V,
              getStatus: widget.underwaterSensorSystem.getIsOn12V,
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
