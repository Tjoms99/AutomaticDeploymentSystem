import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';

class SideMenu extends StatefulWidget {
  ValueNotifier<int> currentPage;

  SideMenu({super.key, required this.currentPage});

  @override
  State<SideMenu> createState() => _SideMenuState();
}

class _SideMenuState extends State<SideMenu> {
  bool homeSelected = true;
  bool winchSelected = false;
  bool temperatureSelected = false;
  bool pressureSelected = false;
  bool depthSelected = false;
  double size = 25;

  void updatePage() {
    homeSelected = false;
    winchSelected = false;
    temperatureSelected = false;
    pressureSelected = false;
    depthSelected = false;
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      width: double.infinity,
      height: SizeConfig.screenHeight,
      color: AppColors.secondaryBackground,
      child: SingleChildScrollView(
          child: Column(
        children: [
          SideMenuButton(
            focused: homeSelected,
            child: IconButton(
              iconSize: size,
              padding: const EdgeInsets.symmetric(vertical: 20),
              icon: Icon(
                Icons.view_quilt,
                color: homeSelected
                    ? AppColors.primaryButton
                    : AppColors.secondaryButton,
              ),
              onPressed: () {
                homeSelected = true;
                winchSelected = false;
                temperatureSelected = false;
                pressureSelected = false;
                depthSelected = false;

                widget.currentPage.value = 0;

                setState(() {});
              },
            ),
          ),
          const SizedBox(height: 10),
          SideMenuButton(
            focused: winchSelected,
            child: IconButton(
              iconSize: size,
              padding: const EdgeInsets.symmetric(vertical: 20),
              icon: Icon(
                Icons.settings,
                color: winchSelected
                    ? AppColors.primaryButton
                    : AppColors.secondaryButton,
              ),
              onPressed: () {
                homeSelected = false;
                winchSelected = true;
                temperatureSelected = false;
                pressureSelected = false;
                depthSelected = false;

                widget.currentPage.value = 1;

                setState(() {});
              },
            ),
          ),
          const SizedBox(height: 10),
          SideMenuButton(
            focused: temperatureSelected,
            child: IconButton(
              iconSize: size,
              padding: const EdgeInsets.symmetric(vertical: 20),
              icon: Icon(
                Icons.thermostat,
                color: temperatureSelected
                    ? AppColors.primaryButton
                    : AppColors.secondaryButton,
              ),
              onPressed: () {
                homeSelected = false;
                winchSelected = false;
                temperatureSelected = true;
                pressureSelected = false;
                depthSelected = false;

                setState(() {});
              },
            ),
          ),
          const SizedBox(height: 10),
          SideMenuButton(
            focused: pressureSelected,
            child: IconButton(
              iconSize: size,
              padding: const EdgeInsets.symmetric(vertical: 20),
              icon: Icon(
                Icons.vertical_align_center,
                color: pressureSelected
                    ? AppColors.primaryButton
                    : AppColors.secondaryButton,
              ),
              onPressed: () {
                homeSelected = false;
                winchSelected = false;
                temperatureSelected = false;
                pressureSelected = true;
                depthSelected = false;

                setState(() {});
              },
            ),
          ),
          const SizedBox(height: 10),
          SideMenuButton(
            focused: depthSelected,
            child: IconButton(
              iconSize: size,
              padding: const EdgeInsets.symmetric(vertical: 20),
              icon: Icon(
                Icons.waves,
                color: depthSelected
                    ? AppColors.primaryButton
                    : AppColors.secondaryButton,
              ),
              onPressed: () {
                homeSelected = false;
                winchSelected = false;
                temperatureSelected = false;
                pressureSelected = false;
                depthSelected = true;

                setState(() {});
              },
            ),
          ),
        ],
      )),
    );
  }
}
