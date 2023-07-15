import 'package:automatic_deployment_system_app/components/control_card.dart';
import 'package:automatic_deployment_system_app/components/control_textfield.dart';
import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/mqtt_topics.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/controllers/system_controller.dart';
import 'package:flutter/material.dart';

class ControlPage extends StatefulWidget {
  final SystemController systemController;
  const ControlPage({super.key, required this.systemController});

  @override
  State<ControlPage> createState() => _ControlPageState();
}

class _ControlPageState extends State<ControlPage> {
  final TextEditingController _targetDepthController = TextEditingController();
  final TextEditingController _targetTimeController = TextEditingController();
  final TextEditingController _samplingIntController = TextEditingController();

  @override
  void initState() {
    _targetDepthController.text =
        widget.systemController.getTargetDepth().toString();
    _targetTimeController.text =
        widget.systemController.getTargetTime().toString();
    _samplingIntController.text =
        widget.systemController.getSamplingInterval().toString();

    super.initState();
  }

  @override
  void dispose() {
    _targetDepthController.dispose();
    _targetTimeController.dispose();
    _samplingIntController.dispose();

    super.dispose();
  }

  void updateTargetDepth() {
    double targetDepth = 0;

    try {
      targetDepth = double.parse(_targetDepthController.text);
      widget.systemController.setTargetDepth(targetDepth);
      widget.systemController.mqtt
          .publishMessage(Topics.targetDepth, targetDepth.toString());
    } catch (e) {}
  }

  void updateTargetTime() {
    int targetTime = 0;

    try {
      targetTime = int.parse(_targetTimeController.text);
      widget.systemController.setTargetTime(targetTime);
      widget.systemController.mqtt
          .publishMessage(Topics.targetTime, targetTime.toString());
    } catch (e) {}
  }

  void updateSamplingInterval() {
    int seconds = 1;
    try {
      seconds = int.parse(_samplingIntController.text);
      widget.systemController.setSamplingInterval(seconds);
      widget.systemController.mqtt
          .publishMessage(Topics.samplingInterval, seconds.toString());
    } catch (e) {}
  }

  void reset() {
    widget.systemController.resetSystem();
    _targetDepthController.text = "0.0";
    _targetTimeController.text = "0";
    _samplingIntController.text = "1";
  }

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
            ControlTextfield(
              label: "TARGET\nDEPTH (m)",
              controller: _targetDepthController,
              callback: updateTargetDepth,
            ),
            ControlTextfield(
              label: "TARGET\nTIME (s)",
              controller: _targetTimeController,
              callback: updateTargetTime,
            ),
            ControlTextfield(
              label: "SAMPLING\nINTERVAL (s)",
              controller: _samplingIntController,
              callback: updateSamplingInterval,
            ),
          ],
        ),
        SizedBox(height: SizeConfig.blockSizeVertical! * 4),
        Wrap(
          runSpacing: 20.0,
          spacing: 20.0,
          alignment: WrapAlignment.start,
          children: [
            Controlcard(
              label: 'RESET SYSTEM',
              textEnable: "",
              textDisable: "",
              enabled: ValueNotifier(true),
              callback: reset,
              getStatus: widget.systemController.getTrue,
            ),
            Controlcard(
              label: 'RESET DEPTH',
              textEnable: "",
              textDisable: "",
              enabled: ValueNotifier(true),
              callback: widget.systemController.resetDepth,
              getStatus: widget.systemController.getTrue,
            ),
            Controlcard(
              label: 'SAMPLING',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.systemController.isSampling,
              callback: widget.systemController.toggleSampling,
              getStatus: widget.systemController.getIsSampling,
            ),
            Controlcard(
              label: 'RS232',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.systemController.underwaterSensorSystem.isOnRS232,
              callback: widget.systemController.toggleRS232,
              getStatus:
                  widget.systemController.underwaterSensorSystem.getIsOnRS232,
            ),
            Controlcard(
              label: '12V',
              textEnable: "ON",
              textDisable: "OFF",
              enabled: widget.systemController.underwaterSensorSystem.isOn12V,
              callback: widget.systemController.toggle12V,
              getStatus:
                  widget.systemController.underwaterSensorSystem.getIsOn12V,
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
