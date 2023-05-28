import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:flutter/material.dart';

class DefaultWidget extends StatefulWidget {
  List<Widget>? widgets;
  DefaultWidget({super.key, required this.widgets});

  @override
  State<DefaultWidget> createState() => _DefaultWidgetState();
}

class _DefaultWidgetState extends State<DefaultWidget> {
  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: double.infinity,
      height: SizeConfig.screenHeight,
      child: SingleChildScrollView(
        padding: const EdgeInsets.symmetric(vertical: 30.0, horizontal: 30.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: widget.widgets ?? [const SizedBox()],
        ),
      ),
    );
  }
}
