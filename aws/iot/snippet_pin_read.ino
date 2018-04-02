// The pin we'll attach the touch-sensor to (in addition to VCC/Ground)
int touch_sensor_pin = 0
setup() {
  // Read the sensor
  int pin = digitalRead(touch_sensor_pin);
  // Print the sensor value
  Serial.print("touch: ");
  Serial.println(pin);
}
