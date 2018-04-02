void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  int desired_led = 1;
  // Write to the LED (1 for on, 0 for off).
  digitalWrite(LED_BUILTIN, desired_led);
}
