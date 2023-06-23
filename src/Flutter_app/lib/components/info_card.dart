import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/controllers/sensor_data.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';

class Infocard extends StatefulWidget {
  final IconData icon;
  final String? label;
  final SensorDataController? sensorData;

  const Infocard({super.key, required this.icon, this.label, this.sensorData});

  @override
  State<Infocard> createState() => _InfocardState();
}

class _InfocardState extends State<Infocard> {
  @override
  Widget build(BuildContext context) {
    return Container(
      constraints: const BoxConstraints(minWidth: 200.0),
      padding: const EdgeInsets.only(
          top: 20.0, left: 20.0, bottom: 20.0, right: 40.0),
      decoration: boxDecoration().getBoxDecoration(),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Icon(
            widget.icon,
            color: AppColors.primaryBackground,
          ),
          SizedBox(
            height: SizeConfig.blockSizeVertical! * 2,
          ),
          PrimaryText(
            text: widget.label!,
            color: AppColors.primaryBackground,
            size: 16.0,
          ),
          SizedBox(
            height: SizeConfig.blockSizeVertical! * 2,
          ),
          ValueListenableBuilder(
            valueListenable: widget.sensorData!.currentData,
            builder: ((context, value, child) {
              return PrimaryText(
                text: widget.sensorData!.getCurrentData().toStringAsFixed(2),
                color: AppColors.primaryBackground,
                size: 18.0,
                fontWeight: FontWeight.w600,
              );
            }),
          )
        ],
      ),
    );
  }
}
