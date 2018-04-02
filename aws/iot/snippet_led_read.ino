void setup() {
  // ...
  long ledValue = digitalRead(LED_BUILTIN);
  Serial.println("PIN:");
  Serial.println(ledValue);
}
