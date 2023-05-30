import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';

class Controlcard extends StatefulWidget {
  final String label;
  bool enabled;
  String textEnable;
  String textDisable;

  Controlcard(
      {super.key,
      required this.label,
      required this.enabled,
      required this.textEnable,
      required this.textDisable});

  @override
  State<Controlcard> createState() => _ControlcardState();
}

class _ControlcardState extends State<Controlcard> {
  bool enabled = false;

  @override
  void initState() {
    super.initState();
  }

  void updateState() {
    enabled = !enabled;
    setState(() {});
  }

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: updateState,
      child: Container(
        constraints: const BoxConstraints(minWidth: 200.0, maxWidth: 200),
        padding: const EdgeInsets.only(
            top: 20.0, left: 20.0, bottom: 20.0, right: 40.0),
        decoration: BoxDecoration(
          borderRadius: BorderRadius.circular(20.0),
          color: enabled ? AppColors.primaryButton : AppColors.secondaryButton,
          boxShadow: const [
            BoxShadow(
              color: Colors.black87,
              blurRadius: 4,
              spreadRadius: 0,
              offset: Offset(4, 4), // Shadow position
            ),
          ],
        ),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            PrimaryText(
              text: widget.label,
              color: AppColors.secondaryBackground,
              size: 20.0,
            ),
            SizedBox(
              height: SizeConfig.blockSizeVertical! * 1,
            ),
            PrimaryText(
              text: enabled ? widget.textEnable : widget.textDisable,
              color: AppColors.primaryBackground,
              size: 16.0,
            ),
          ],
        ),
      ),
    );
  }
}
