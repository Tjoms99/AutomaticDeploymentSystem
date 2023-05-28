import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';

class SideMenuInfo extends StatefulWidget {
  SideMenuInfo({super.key});

  @override
  State<SideMenuInfo> createState() => _SideMenuInfoState();
}

class _SideMenuInfoState extends State<SideMenuInfo> {
  TextEditingController _targetDepthController =
      TextEditingController(text: "-10 m");
  TextEditingController _depthDifferenceController =
      TextEditingController(text: "0.23 m");
  TextEditingController _samplingIntervalController =
      TextEditingController(text: "1 s");
  TextEditingController _samplingStatusController =
      TextEditingController(text: "Sampling");
  TextEditingController _rs232StatusController =
      TextEditingController(text: "OFF");
  TextEditingController _12VStatusController =
      TextEditingController(text: "OFF");

  @override
  Widget build(BuildContext context) {
    return Container(
      color: AppColors.secondaryBackground,
      child: DefaultWidget(
        widgets: [
          Header(label: "Info", enableUndertext: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "TARGET DEPTH:"),
          PrimaryTextfield(controller: _targetDepthController),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "DEPTH DIFFERENCE:"),
          PrimaryTextfield(
              controller: _depthDifferenceController, enabled: false),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "SAMPLING INTERVAL:"),
          PrimaryTextfield(controller: _samplingIntervalController),
          SizedBox(height: SizeConfig.blockSizeVertical! * 4),
          const PrimaryText(text: "SAMPLING STATUS:"),
          PrimaryTextfield(
              controller: _samplingStatusController, enabled: false),
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
