import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:flutter/material.dart';

class PrimaryText extends StatelessWidget {
  final double size;
  final FontWeight fontWeight;
  final Color color;
  final String text;
  final double height;

  const PrimaryText(
      {super.key,
      required this.text,
      this.fontWeight = FontWeight.w400,
      this.color = AppColors.primaryText,
      this.size = 20,
      this.height = 1.3});

  @override
  Widget build(BuildContext context) {
    return Text(
      text,
      style: TextStyle(
          color: color,
          height: height,
          fontFamily: 'openSans',
          fontSize: size,
          fontWeight: fontWeight),
    );
  }
}

class PrimaryTextfield extends StatelessWidget {
  final double size;
  final FontWeight fontWeight;
  final Color color;
  final double height;
  final TextEditingController controller;
  final bool enabled;

  const PrimaryTextfield({
    super.key,
    this.fontWeight = FontWeight.w400,
    this.color = AppColors.primaryText,
    this.size = 20,
    this.height = 1.3,
    required this.controller,
    this.enabled = true,
  });

  @override
  Widget build(BuildContext context) {
    return TextField(
      controller: controller,
      enabled: enabled,
      style: TextStyle(
          color: color,
          height: height,
          fontFamily: 'openSans',
          fontSize: size,
          fontWeight: fontWeight),
    );
  }
}

class SideMenuButton extends StatefulWidget {
  final IconButton child;
  final bool focused;

  const SideMenuButton({super.key, required this.child, required this.focused});

  @override
  State<SideMenuButton> createState() => _SideMenuButtonState();
}

class _SideMenuButtonState extends State<SideMenuButton> {
  @override
  Widget build(BuildContext context) {
    return Container(
      width: SizeConfig.screenWidth,
      decoration: BoxDecoration(
        color: AppColors.secondaryBackground,
        boxShadow: [
          widget.focused
              ? const BoxShadow(
                  color: Colors.black87,
                  blurRadius: 4,
                  spreadRadius: 0,
                  offset: Offset(5, 0), // Shadow position
                )
              : const BoxShadow(),
        ],
      ),
      child: widget.child,
    );
  }
}
