// initialize pins
int waterSensorHighPin = 6;
int waterSensorLowPin = 7;

int waterSensorHighStatus;
int waterSensorLowStatus;

void setup() {
  Serial.begin(9600);

  pinMode(waterSensorHighPin, INPUT);
  pinMode(waterSensorLowPin, INPUT);
}

void loop() {
  waterSensorLowStatus = digitalRead(waterSensorLowPin);
  waterSensorHighStatus = digitalRead(waterSensorHighPin);

  Serial.println(waterSensorLowStatus);
  Serial.println(waterSensorHighStatus);
  
  delay(100);
}
