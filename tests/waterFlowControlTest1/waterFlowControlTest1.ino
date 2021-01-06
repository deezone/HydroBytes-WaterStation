const int RELAY_ENABLED = 2;

void setup() {
  pinMode(RELAY_ENABLED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  Serial.println("Relay ON");
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(RELAY_ENABLED, LOW); // The relay is active low, it turns on when set to LOW
  delay(1000);

  Serial.println("Relay OFF");
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(RELAY_ENABLED, HIGH);
  delay(1000);
}
