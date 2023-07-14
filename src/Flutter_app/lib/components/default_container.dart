import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:flutter/material.dart';

class DefaultContainer extends StatefulWidget {
  final Widget child;
  const DefaultContainer({super.key, required this.child});

  @override
  State<DefaultContainer> createState() => _DefaultContainerState();
}

class _DefaultContainerState extends State<DefaultContainer> {
  @override
  Widget build(BuildContext context) {
    return Container(
      alignment: Alignment.centerLeft,
      width: SizeConfig.screenWidth,
      constraints: const BoxConstraints(minWidth: 200.0, maxWidth: 200),
      padding: const EdgeInsets.only(
          top: 20.0, left: 20.0, bottom: 20.0, right: 40.0),
      decoration: BoxDecoration(
        borderRadius: BorderRadius.circular(20.0),
        color: AppColors.primaryButton,
        boxShadow: const [
          BoxShadow(
            color: Colors.black87,
            blurRadius: 4,
            spreadRadius: 0,
            offset: Offset(4, 4), // Shadow position
          ),
        ],
      ),
      child: widget.child,
    );
  }
}
