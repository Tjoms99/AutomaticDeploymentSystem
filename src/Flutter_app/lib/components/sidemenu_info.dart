import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/controllers/USS_controller.dart';
import 'package:automatic_deployment_system_app/controllers/system_controller.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';

class SideMenuInfo extends StatefulWidget {
  final SystemController systemController;
  const SideMenuInfo({super.key, required this.systemController});

  @override
  State<SideMenuInfo> createState() => _SideMenuInfoState();
}

class _SideMenuInfoState extends State<SideMenuInfo> {
  final TextEditingController _systemController = TextEditingController();
  final TextEditingController _statusController = TextEditingController();
  final TextEditingController _targetDepthController = TextEditingController();
  final TextEditingController _depthDiffController = TextEditingController();
  final TextEditingController _targetTimeController = TextEditingController();
  final TextEditingController _timeLeftController = TextEditingController();
  final TextEditingController _samplingIntController = TextEditingController();
  final TextEditingController _rs232StatusController = TextEditingController();
  final TextEditingController _12VStatusController = TextEditingController();

  bool sampling = false;

  @override
  void initState() {
    _targetDepthController.text = "10";
    _targetTimeController.text = "100";
    _samplingIntController.text = "1";

    widget.systemController.registerCallback(updateDepthDifference);
    widget.systemController.registerCallback(updateSamplingInterval);
    widget.systemController.registerCallback(updateTimeLeft);
    widget.systemController.registerCallback(updateSystem);
    widget.systemController.registerCallback(updateStatus);
    widget.systemController.registerCallback(updateRS232);
    widget.systemController.registerCallback(update12V);

    super.initState();
  }

  void updateDepthDifference() {
    double targetDepth = 0;
    double currentDepth = 0;
    double depthDifference = 0;

    try {
      targetDepth = double.parse(_targetDepthController.text);
      currentDepth = widget.systemController.underwaterSensorSystem
          .getDepthSensor()
          .currentData
          .value;
    } catch (e) {}

    depthDifference = targetDepth - currentDepth;
    _depthDiffController.text = "${depthDifference.toStringAsFixed(2)} m";
  }

  void updateTimeLeft() {
    int targetTime = 0;
    int currentTime = 0;

    try {
      targetTime = int.parse(_targetTimeController.text);
      currentTime = widget.systemController.getCurrentTime();
    } catch (e) {}

    int timeLeft = targetTime - currentTime;
    widget.systemController.setTimeLeft(timeLeft);

    _timeLeftController.text = "${timeLeft.toStringAsFixed(2)} s";
    setSampling();
  }

  void updateSamplingInterval() {
    int seconds = 1;

    try {
      seconds = int.parse(_samplingIntController.text);
    } catch (e) {}

    widget.systemController.updateTimer(seconds);
  }

  void updateSystem() {
    widget.systemController.getIsOnSystem()
        ? _systemController.text = "ACTIVE"
        : _systemController.text = "INACTIVE";
  }

  void updateStatus() {
    widget.systemController.getIsSampling()
        ? _statusController.text = "SAMPLING"
        : _statusController.text = "IDLE";
  }

  void updateRS232() {
    widget.systemController.underwaterSensorSystem.getIsOnRS232()
        ? _rs232StatusController.text = "ON"
        : _rs232StatusController.text = "OFF";
  }

  void update12V() {
    widget.systemController.underwaterSensorSystem.getIsOn12V()
        ? _12VStatusController.text = "ON"
        : _12VStatusController.text = "OFF";
  }

  void setSampling() {
    sampling = widget.systemController.getIsSampling();
    setState(() {});
  }

  @override
  Widget build(BuildContext context) {
    setSampling();

    return Container(
      color: AppColors.secondaryBackground,
      child: DefaultWidget(
        widgets: [
          const Header(label: "Info", enableUndertext: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "SYSTEM"),
          PrimaryTextfield(controller: _systemController, enabled: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "STATUS"),
          PrimaryTextfield(controller: _statusController, enabled: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "TARGET DEPTH"),
          PrimaryTextfield(
              controller: _targetDepthController, enabled: !sampling),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "DEPTH DIFFERENCE"),
          PrimaryTextfield(controller: _depthDiffController, enabled: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "TARGET TIME"),
          PrimaryTextfield(
              controller: _targetTimeController,
              enabled: !widget.systemController.getIsSampling()),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "TIME LEFT"),
          PrimaryTextfield(controller: _timeLeftController, enabled: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "SAMPLING INTERVAL"),
          PrimaryTextfield(
              controller: _samplingIntController, enabled: !sampling),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "RS232"),
          PrimaryTextfield(controller: _rs232StatusController, enabled: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "12V"),
          PrimaryTextfield(controller: _12VStatusController, enabled: false),
        ],
      ),
    );
  }
}
