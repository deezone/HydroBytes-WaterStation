#include <SoftwareSerial.h>
SoftwareSerial mySerial(2,3);

/**
 * setup()
 */
void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  
  Serial.println("Hello World (Sensors)");
  delay(5000);
}

/**
 * loop() - event loop
 */
void loop() {
  String incomingString="";
  boolean stringReady = false;

  /**
   * Read incoming serial communication from server (esp8266-esp-01)
   */
  while(mySerial.available()) {
    incomingString = mySerial.readString();
    stringReady = true;
  }

  /**
   * Respond and send requests to server (esp8266-esp-01)
   */
  if(stringReady) {
    Serial.println("Uno (sensors) received from ESP (server): " + incomingString);
  }
}
