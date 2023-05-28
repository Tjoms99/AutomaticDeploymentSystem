import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';

class Controlcard extends StatefulWidget {
  final IconData icon;
  final String? label;

  const Controlcard({super.key, required this.icon, required this.label});

  @override
  State<Controlcard> createState() => _ControlcardState();
}

class _ControlcardState extends State<Controlcard> {
  final TextEditingController _controller = TextEditingController();
  @override
  Widget build(BuildContext context) {
    return Container(
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
            height: SizeConfig.blockSizeVertical! * 1,
          ),
          TextField(
            controller: _controller,
          ),
        ],
      ),
    );
  }
}
