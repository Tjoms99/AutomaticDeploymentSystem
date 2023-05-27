import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';

class Header extends StatelessWidget {
  const Header({super.key});

  @override
  Widget build(BuildContext context) {
    return const Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        PrimaryText(
          text: 'Dashboard',
          size: 30.0,
          fontWeight: FontWeight.w700,
        ),
        PrimaryText(
          text: 'Automatic Deployment System',
          size: 16.0,
          color: AppColors.primaryText,
        ),
      ],
    );
  }
}
