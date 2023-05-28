import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';

class Header extends StatelessWidget {
  String label = "";
  bool enableUndertext;

  Header({super.key, required this.label, required this.enableUndertext});

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        PrimaryText(
          text: label,
          size: 30.0,
          fontWeight: FontWeight.w700,
        ),
        enableUndertext
            ? const PrimaryText(
                text: 'Automatic Deployment System',
                size: 16.0,
                color: AppColors.primaryText,
              )
            : SizedBox(),
      ],
    );
  }
}
