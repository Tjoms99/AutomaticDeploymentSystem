import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/mqtt_topics.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
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
    widget.systemController.registerCallback(updateTimeLeft);
    widget.systemController.registerCallback(updateSystem);
    widget.systemController.registerCallback(updateStatus);
    widget.systemController.registerCallback(updateRS232);
    widget.systemController.registerCallback(update12V);

    super.initState();
  }

  void updateTargetDepth() {
    double targetDepth = 0;

    try {
      targetDepth = double.parse(_targetDepthController.text);
      widget.systemController.mqtt
          .publishMessage(Topics.targetDepth, targetDepth.toString());
    } catch (e) {}
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

  void updateTargetTime() {
    int targetTime = 0;

    try {
      targetTime = int.parse(_targetTimeController.text);
      widget.systemController.mqtt
          .publishMessage(Topics.targetTime, targetTime.toString());
    } catch (e) {}
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
      widget.systemController.mqtt
          .publishMessage(Topics.samplingInterval, seconds.toString());
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
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              const Header(label: "Info", enableUndertext: false),
              ValueListenableBuilder(
                valueListenable: widget.systemController.mqtt.isConnected,
                builder: (context, value, child) {
                  return widget.systemController.mqtt.isConnected.value
                      ? IconButton(
                          onPressed: widget.systemController.mqtt.disconnect,
                          icon: const Icon(
                            Icons.signal_wifi_statusbar_4_bar,
                            color: Colors.green,
                          ),
                        )
                      : IconButton(
                          onPressed: widget.systemController.mqtt.connect,
                          icon: const Icon(
                            Icons.signal_wifi_off,
                            color: Colors.red,
                          ),
                        );
                },
              )
            ],
          ),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "SYSTEM"),
          PrimaryTextfield(
            controller: _systemController,
            enabled: false,
            onChangedFunction: () {},
          ),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "STATUS"),
          PrimaryTextfield(
            controller: _statusController,
            enabled: false,
            onChangedFunction: () {},
          ),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "TARGET DEPTH"),
          PrimaryTextfield(
            controller: _targetDepthController,
            enabled: !sampling,
            onChangedFunction: updateTargetDepth,
          ),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "DEPTH DIFFERENCE"),
          PrimaryTextfield(
            controller: _depthDiffController,
            enabled: false,
            onChangedFunction: () {},
          ),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "TARGET TIME"),
          PrimaryTextfield(
            controller: _targetTimeController,
            enabled: !widget.systemController.getIsSampling(),
            onChangedFunction: updateTargetTime,
          ),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "TIME LEFT"),
          PrimaryTextfield(
            controller: _timeLeftController,
            enabled: false,
            onChangedFunction: () {},
          ),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "SAMPLING INTERVAL"),
          PrimaryTextfield(
            controller: _samplingIntController,
            enabled: !sampling,
            onChangedFunction: updateSamplingInterval,
          ),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "RS232"),
          PrimaryTextfield(
            controller: _rs232StatusController,
            enabled: false,
            onChangedFunction: () {},
          ),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "12V"),
          PrimaryTextfield(
            controller: _12VStatusController,
            enabled: false,
            onChangedFunction: () {},
          ),
        ],
      ),
    );
  }
}
