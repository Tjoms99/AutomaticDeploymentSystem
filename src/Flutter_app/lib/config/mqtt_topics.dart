// MQTT Topics
abstract class Topics {
  //--------------------------------------------------------------------
  // APPLICATION TOPICS
  //Publish topics
  static const String system = "AutomaticDeploymentSystem/app/system";
  static const String sampling = "AutomaticDeploymentSystem/app/sampling";
  static const String targetDepth =
      "AutomaticDeploymentSystem/app/target_depth";
  static const String targetTime = "AutomaticDeploymentSystem/app/target_time";

  //--------------------------------------------------------------------
  // UNDERWATER SENSOR SYSTEM TOPICS
  //Subscribe topics
  static const String depth = "AutomaticDeploymentSystem/USS/depth";
  static const String temperature = "AutomaticDeploymentSystem/USS/temperature";
  static const String pressure = "AutomaticDeploymentSystem/USS/pressure";
  static const String battery = "AutomaticDeploymentSystem/USS/battery";
  //Publish topics
  static const String depthInit = "AutomaticDeploymentSystem/USS/depth_init";
  static const String rs232 = "AutomaticDeploymentSystem/USS/rs232";
  static const String volt = "AutomaticDeploymentSystem/USS/12v";
  static const String samplingInterval =
      "AutomaticDeploymentSystem/USS/sampling_interval";
}
