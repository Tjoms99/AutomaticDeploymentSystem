import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/controllers/sensor_data.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';
import 'package:syncfusion_flutter_charts/charts.dart';

class Infograph extends StatefulWidget {
  final String? label;
  final String? labelAxisy;
  final double? axisYMinimum;
  final double? axisYMaximum;
  final SensorDataController sensorData;

  const Infograph({
    super.key,
    this.label,
    this.labelAxisy,
    required this.sensorData,
    required this.axisYMinimum,
    required this.axisYMaximum,
  });

  @override
  State<Infograph> createState() => _InfographState();
}

class _InfographState extends State<Infograph> {
  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        SizedBox(height: SizeConfig.blockSizeVertical! * 2),
        Container(
          constraints: const BoxConstraints(maxWidth: 850.0, maxHeight: 400),
          padding: const EdgeInsets.only(
              top: 20.0, left: 20.0, bottom: 20.0, right: 40.0),
          decoration: boxDecoration().getBoxDecoration(),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              PrimaryText(
                text: widget.label!,
                color: AppColors.primaryBackground,
                size: 16.0,
              ),
              SizedBox(
                height: SizeConfig.blockSizeVertical! * 2,
              ),
              SfCartesianChart(
                backgroundColor: AppColors.primaryButton,
                primaryXAxis: NumericAxis(
                  title: AxisTitle(
                      text: "Time in seconds",
                      textStyle:
                          const TextStyle(color: AppColors.primaryBackground)),
                  //autoScrollingDelta: 300,
                ),
                primaryYAxis: NumericAxis(
                  title: AxisTitle(
                      text: widget.labelAxisy ?? "",
                      textStyle:
                          const TextStyle(color: AppColors.primaryBackground)),
                  minimum: widget.axisYMinimum ?? 0,
                  maximum: widget.axisYMaximum ?? 100,
                ),
                series: <ChartSeries>[
                  widget.sensorData.getDataSeries(),
                ],
              ),
            ],
          ),
        ),
      ],
    );
  }
}
