#include <SoftwareSerial.h>
SoftwareSerial mySerial(2,3);

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  
  Serial.println("Hello World (UNO)");
  delay(5000);
}

void loop() {
  String incomingString="";
  boolean stringReady = false;

  while(mySerial.available()) {
    incomingString = mySerial.readString();
    stringReady = true;
  }

  if(stringReady) {
    Serial.println("Uno received from ESP: " + incomingString);
  }

}
