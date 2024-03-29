import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';

class Controlcard extends StatefulWidget {
  final String label;
  final String textEnable;
  final String textDisable;
  final Function() callback;
  final Function() getStatus;
  final ValueNotifier<bool> enabled;

  const Controlcard(
      {super.key,
      required this.label,
      required this.textEnable,
      required this.textDisable,
      required this.callback,
      required this.getStatus,
      required this.enabled});

  @override
  State<Controlcard> createState() => _ControlcardState();
}

class _ControlcardState extends State<Controlcard> {
  void updateState() {
    widget.callback();
    setState(() {});
  }

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: updateState,
      child: ValueListenableBuilder(
        valueListenable: widget.enabled,
        builder: ((context, value, child) {
          return Container(
            constraints: const BoxConstraints(minWidth: 200.0, maxWidth: 200),
            padding: const EdgeInsets.only(
                top: 20.0, left: 20.0, bottom: 20.0, right: 40.0),
            decoration: boxDecoration()
                .getControlBoxDecoration(() => widget.getStatus()),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                PrimaryText(
                  text: widget.label,
                  color: AppColors.secondaryBackground,
                  size: 20.0,
                  fontWeight: FontWeight.w500,
                ),
                SizedBox(
                  height: SizeConfig.blockSizeVertical! * 1,
                ),
                PrimaryText(
                  text: widget.getStatus()
                      ? widget.textEnable
                      : widget.textDisable,
                  color: AppColors.primaryBackground,
                  size: 16.0,
                  fontWeight: FontWeight.w500,
                ),
              ],
            ),
          );
        }),
      ),
    );
  }
}
