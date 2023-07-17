import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:flutter/material.dart';

class DefaultControlLayout extends StatefulWidget {
  final List<Widget>? widgets;
  final String label;

  const DefaultControlLayout({
    super.key,
    required this.widgets,
    required this.label,
  });

  @override
  State<DefaultControlLayout> createState() => _DefaultControlLayoutState();
}

class _DefaultControlLayoutState extends State<DefaultControlLayout> {
  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        SizedBox(height: SizeConfig.blockSizeVertical! * 4),
        Header(label: widget.label, enableUndertext: false, textSize: 20),
        SizedBox(height: SizeConfig.blockSizeVertical! * 1),
        Wrap(
          runSpacing: 20.0,
          spacing: 20.0,
          alignment: WrapAlignment.start,
          children: widget.widgets ?? [const SizedBox()],
        ),
      ],
    );
  }
}
