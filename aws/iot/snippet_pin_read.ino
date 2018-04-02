int touch_sensor_pin = 7
setup() {
  // Read the sensor
  int pin = digitalRead(touch_sensor_pin);
  // Print the sensor value
  Serial.print("touch: ");
  Serial.println(pin);
}
