import 'package:automatic_deployment_system_app/config/size_config.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:automatic_deployment_system_app/style/style.dart';
import 'package:flutter/material.dart';

class SideMenuButtons extends StatefulWidget {
  final ValueNotifier<int> currentPage;

  const SideMenuButtons({super.key, required this.currentPage});

  @override
  State<SideMenuButtons> createState() => _SideMenuButtonsState();
}

class _SideMenuButtonsState extends State<SideMenuButtons> {
  bool homeSelected = true;
  bool winchSelected = false;
  bool dataSelected = false;

  double size = 25;

  void updatePage(int index) {
    homeSelected = false;
    winchSelected = false;
    dataSelected = false;

    index == 0
        ? homeSelected = true
        : index == 1
            ? winchSelected = true
            : dataSelected = true;
    setState(() {});
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
                widget.currentPage.value = 0;
                updatePage(widget.currentPage.value);
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
                widget.currentPage.value = 1;
                updatePage(widget.currentPage.value);
              },
            ),
          ),
          const SizedBox(height: 10),
          SideMenuButton(
            focused: dataSelected,
            child: IconButton(
              iconSize: size,
              padding: const EdgeInsets.symmetric(vertical: 20),
              icon: Icon(
                Icons.dataset,
                color: dataSelected
                    ? AppColors.primaryButton
                    : AppColors.secondaryButton,
              ),
              onPressed: () {
                widget.currentPage.value = 2;

                updatePage(widget.currentPage.value);
              },
            ),
          ),
        ],
      )),
    );
  }
}
