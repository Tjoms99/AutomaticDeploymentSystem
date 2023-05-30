import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/data/underwater_sensor_system.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';

class SideMenuInfo extends StatefulWidget {
  final UnderwaterSensorSystem underwaterSensorSytem;
  const SideMenuInfo({super.key, required this.underwaterSensorSytem});

  @override
  State<SideMenuInfo> createState() => _SideMenuInfoState();
}

class _SideMenuInfoState extends State<SideMenuInfo> {
  final TextEditingController _targetDepthController =
      TextEditingController(text: "-10");
  final TextEditingController _depthDifferenceController =
      TextEditingController(text: "0.23");
  final TextEditingController _targetTimeController =
      TextEditingController(text: "60");
  final TextEditingController _timeLeftController =
      TextEditingController(text: "20");
  final TextEditingController _samplingIntervalController =
      TextEditingController(text: "1");
  final TextEditingController _systemStatusController =
      TextEditingController(text: "Sampling");
  final TextEditingController _rs232StatusController =
      TextEditingController(text: "OFF");
  final TextEditingController _12VStatusController =
      TextEditingController(text: "OFF");

  @override
  void initState() {
    widget.underwaterSensorSytem.registerCallback(updateDepthDifference);
    widget.underwaterSensorSytem.registerCallback(updateSamplingInterval);
    widget.underwaterSensorSytem.registerCallback(updateTimeLeft);
    widget.underwaterSensorSytem.registerCallback(updateSystem);
    widget.underwaterSensorSytem.registerCallback(updateRS232);
    widget.underwaterSensorSytem.registerCallback(update12V);

    super.initState();
  }

  void updateDepthDifference() {
    try {
      double depthDifference = double.parse(_targetDepthController.text) -
          widget.underwaterSensorSytem.getDepthSensor().currentData.value;
      _depthDifferenceController.text =
          "${depthDifference.toStringAsFixed(2)} m";
    } catch (e) {}
  }

  void updateTimeLeft() {
    try {
      double timeLeft = double.parse(_targetTimeController.text) -
          widget.underwaterSensorSytem.getCurrentTime();
      _timeLeftController.text = "${timeLeft.toStringAsFixed(2)} s";
    } catch (e) {}
  }

  void updateSamplingInterval() {
    try {
      int seconds = int.parse(_samplingIntervalController.text);
      widget.underwaterSensorSytem.updateTimer(seconds);
    } catch (e) {}
  }

  void updateSystem() {
    widget.underwaterSensorSytem.isOnSystem()
        ? _systemStatusController.text = "ACTIVE"
        : _systemStatusController.text = "INACTIVE";
  }

  void updateRS232() {
    widget.underwaterSensorSytem.isOnRS232()
        ? _rs232StatusController.text = "ON"
        : _rs232StatusController.text = "OFF";
  }

  void update12V() {
    widget.underwaterSensorSytem.isOn12V()
        ? _12VStatusController.text = "ON"
        : _12VStatusController.text = "OFF";
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      color: AppColors.secondaryBackground,
      child: DefaultWidget(
        widgets: [
          const Header(label: "Info", enableUndertext: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "SYSTEM STATUS:"),
          PrimaryTextfield(controller: _systemStatusController, enabled: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "TARGET DEPTH:"),
          PrimaryTextfield(controller: _targetDepthController),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "DEPTH DIFFERENCE:"),
          PrimaryTextfield(
              controller: _depthDifferenceController, enabled: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "TARGET TIME:"),
          PrimaryTextfield(controller: _targetTimeController),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "TIME LEFT:"),
          PrimaryTextfield(controller: _timeLeftController, enabled: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "SAMPLING INTERVAL:"),
          PrimaryTextfield(controller: _samplingIntervalController),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "RS232 STATUS:"),
          PrimaryTextfield(controller: _rs232StatusController, enabled: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "12V STATUS:"),
          PrimaryTextfield(controller: _12VStatusController, enabled: false),
        ],
      ),
    );
  }
}
