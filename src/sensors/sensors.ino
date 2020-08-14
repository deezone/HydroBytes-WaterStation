#include <SoftwareSerial.h>
SoftwareSerial serverSerial(10, 11);

uint8_t pin_led = 12;

/**
 * setup()
 */
void setup() {
  Serial.begin(9600);
  Serial.println("HydroBites Water Station (Sensors) serial connection started...");
  delay(1000);

  Serial.println("HydroBites Water Station (Sensors <-> Server) serial connection started...");
  serverSerial.begin(9600);
  
  Serial.println("HydroBites Water Station (Sensors) setup complete...");
}

/**
 * loop() - event loop
 */
void loop() {
  String incomingString="";
  boolean stringReady = false;

  // Send test "PONG"
  serverSerial.println("Sensor PONG");

  while(serverSerial.available()) {
    incomingString = serverSerial.readStringUntil('\n');
    Serial.println((String)"from server: " + incomingString);
  }
}
