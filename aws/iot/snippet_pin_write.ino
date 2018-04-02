// The pin we'll attach the buzzer to (in addition to VCC/Ground)
int buzzer_pin = 7;

void setup() {
  pinMode(buzzer_pin, OUTPUT);
  // Write high - 1
  digitalWrite(buzzer_pin, HIGH);
  delay(100);
  // Write low - 0
  digitalWrite(buzzer_pin, LOW);
  delay(100);
  // Write high - 1
  digitalWrite(buzzer_pin, HIGH);
}
