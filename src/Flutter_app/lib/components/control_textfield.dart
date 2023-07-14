import 'package:automatic_deployment_system_app/components/default_container.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';

class ControlTextfield extends StatefulWidget {
  final String label;
  final TextEditingController controller;
  final Function() callback;

  const ControlTextfield({
    super.key,
    required this.label,
    required this.controller,
    required this.callback,
  });

  @override
  State<ControlTextfield> createState() => _ControlTextfieldState();
}

class _ControlTextfieldState extends State<ControlTextfield> {
  @override
  Widget build(BuildContext context) {
    return DefaultContainer(
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          PrimaryText(text: widget.label, color: AppColors.primaryBackground),
          PrimaryTextfield(
            color: AppColors.primaryBackground,
            controller: widget.controller,
            onChangedFunction: widget.callback,
          ),
        ],
      ),
    );
  }
}
