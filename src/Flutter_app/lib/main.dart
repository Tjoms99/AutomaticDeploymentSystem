import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'pages/parent.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    SizeConfig().init(context);
    SystemChrome.setPreferredOrientations([
      DeviceOrientation.landscapeLeft,
      DeviceOrientation.landscapeRight,
    ]);
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: "Automatic Deployment System",
      theme: ThemeData(
          primarySwatch: Colors.blue,
          scaffoldBackgroundColor: AppColors.primaryBackground),
      home: const ParentPage(),
    );
  }
}
