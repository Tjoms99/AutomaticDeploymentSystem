import 'package:automatic_deployment_system_app/components/data_table.dart';
import 'package:automatic_deployment_system_app/components/default_container.dart';
import 'package:automatic_deployment_system_app/controllers/system_controller.dart';
import 'package:automatic_deployment_system_app/style/colors.dart';
import 'package:flutter/material.dart';

class DataSampleTableExpand extends StatefulWidget {
  final SystemController systemController;
  const DataSampleTableExpand({super.key, required this.systemController});

  @override
  State<DataSampleTableExpand> createState() => _DataSampleTableExpandState();
}

class _DataSampleTableExpandState extends State<DataSampleTableExpand> {
  bool expand = false;
  String time =
      '${DateTime.now().hour.toString()} : ${DateTime.now().minute.toString()} : ${DateTime.now().second.toString()}';
  String date =
      '${DateTime.now().day.toString()} / ${DateTime.now().month.toString()} / ${DateTime.now().year.toString()}';

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(vertical: 10, horizontal: 0),
      child: GestureDetector(
        onTap: () {
          expand = !expand;
          setState(() {});
        },
        child: expand
            ? Column(
                crossAxisAlignment: CrossAxisAlignment.center,
                children: [
                  DefaultContainer(
                    child: Text(
                      time,
                      style: const TextStyle(
                        color: AppColors.primaryBackground,
                        height: 1.3,
                        fontFamily: 'openSans',
                        fontSize: 20,
                        fontWeight: FontWeight.w500,
                      ),
                    ),
                  ),
                  DataSampleTable(systemController: widget.systemController),
                ],
              )
            : DefaultContainer(
                child: Text(
                  date,
                  style: const TextStyle(
                    color: AppColors.primaryBackground,
                    height: 1.3,
                    fontFamily: 'openSans',
                    fontSize: 20,
                    fontWeight: FontWeight.w500,
                  ),
                ),
              ),
      ),
    );
  }
}
