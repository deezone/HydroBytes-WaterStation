#include <SoftwareSerial.h>
SoftwareSerial serverSerial(10, 11);

uint8_t pin_led = 12;

/**
 * setup()
 */
void setup() {
  Serial.println("HydroBites Water Station");
  Serial.println("v0.0.2");
  Serial.println("");

  Serial.begin(9600);
  Serial.println("Sensors: serial connection started...");

  Serial.println("Sensors: Sensors <-> Server serial connection started...");
  serverSerial.begin(9600);
  
  Serial.println("Sensors: setup complete...");
  Serial.flush();
}

/**
 * loop() - event loop
 */
void loop() {
  String serverMessage;

  while(serverSerial.available()) {
    serverMessage = serverSerial.readStringUntil('\n');
    Serial.println((String)"Server: " + serverMessage);

    // Router for sensor actions
    if (serverMessage.indexOf("GET /status") >= 0) {
      // Send Serial message back to server to confirm receipt
      serverSerial.println("serial_status: true");

      // Log to serial monitor
      Serial.println("serial_status: true");
    }
  }
}
