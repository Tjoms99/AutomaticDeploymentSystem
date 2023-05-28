import 'package:automatic_deployment_system_app/components/control_card.dart';
import 'package:automatic_deployment_system_app/components/defualt_widget.dart';
import 'package:automatic_deployment_system_app/components/header.dart';
import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:flutter/material.dart';

class ControlPage extends StatefulWidget {
  const ControlPage({super.key});

  @override
  State<ControlPage> createState() => _ControlPageState();
}

class _ControlPageState extends State<ControlPage> {
  @override
  Widget build(BuildContext context) {
    return DefaultWidget(
      widgets: [
        Header(label: 'Control', enableUndertext: true),
        SizedBox(height: SizeConfig.blockSizeVertical! * 4),
        const Wrap(
          runSpacing: 20.0,
          spacing: 20.0,
          alignment: WrapAlignment.start,
          children: [
            Controlcard(icon: Icons.waves, label: 'Target Depth'),
            Controlcard(icon: Icons.waves, label: 'Target Depth'),
            Controlcard(icon: Icons.waves, label: 'Target Depth'),
            Controlcard(icon: Icons.waves, label: 'Target Depth'),
            Controlcard(icon: Icons.waves, label: 'Target Depth'),
            Controlcard(icon: Icons.waves, label: 'Target Depth'),
            Controlcard(icon: Icons.waves, label: 'Target Depth'),
            Controlcard(icon: Icons.waves, label: 'Target Depth'),
            Controlcard(icon: Icons.waves, label: 'Target Depth'),
            Controlcard(icon: Icons.waves, label: 'Target Depth')
          ],
        ),
      ],
    );
  }
}
