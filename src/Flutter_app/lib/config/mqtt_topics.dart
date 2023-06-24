// MQTT Topics
abstract class Topics {
  // Controll
  static const String system = "AutomaticDeploymentSystem/app/system";
  static const String sampling = "AutomaticDeploymentSystem/app/sampling";
  static const String rs232 = "AutomaticDeploymentSystem/app/rs232";
  static const String volt = "AutomaticDeploymentSystem/app/12v";

  // Data
  static const String depth = "AutomaticDeploymentSystem/USS/depth";
  static const String temperature = "AutomaticDeploymentSystem/USS/temperature";
  static const String pressure = "AutomaticDeploymentSystem/USS/pressure";
  static const String battery = "AutomaticDeploymentSystem/USS/battery";
}
