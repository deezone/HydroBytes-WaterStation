#include <SoftwareSerial.h>
SoftwareSerial serverSerial(10, 11);

// initialize pins
uint8_t pin_led = 12;

int waterSensorHighPin = 6;
int waterSensorMidPin  = 7;
int waterSensorLowPin  = 8;

/**
 * setup()
 */
void setup() {
  Serial.begin(9600);
  Serial.println("Sensors: serial connection started...");
  Serial.println("");

  Serial.println("HydroBites Water Station");
  Serial.println("v0.1.0");
  Serial.println("");

  Serial.println("Sensors: Sensors <-> Server serial connection started...");
  serverSerial.begin(9600);
  
  Serial.println("Sensors: setup complete...");

    // Assign pins
  pinMode(pin_led, OUTPUT);

  pinMode(waterSensorHighPin, INPUT);
  pinMode(waterSensorMidPin,  INPUT);
  pinMode(waterSensorLowPin,  INPUT);

  Serial.println("Sensors: pin assignment complete...");
  Serial.println("");

  /**
   * Serial.write are transmitted in the background (from an interrupt handler) allowing your
   * sketch code to immediately resume processing. This is usually a good thing (it can make the
   * sketch more responsive) but sometimes you want to wait until all characters are sent. You can
   * achieve this by calling Serial.flush() immediately following Serial.write().
   */
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

    } else if (serverMessage.indexOf("GET /led/toggle") >= 0) {
      int ledState = toggleLed();
      sendLedStatus(ledState);
    } else if (serverMessage.indexOf("GET /led") >= 0) {
      int ledState = ledStatus();
      sendLedStatus(ledState);
    } else if (serverMessage.indexOf("GET /water") >= 0) {
      int waterLevel = waterLevel();
      sendWaterLevelStatus(waterLevel);
    }
  }
}

/**
 * Get the current state of the LED - on (1)/off (0)
 */
int ledStatus() {
  int ledState = 0;

  // Get LED reading
  ledState = digitalRead(pin_led);

  return ledState;
}

/**
 * Adjust the state of the LED between on/off
 */
int toggleLed() {
  int setLedState = 0;

  // Write the opposite of the current state
  setLedState = !digitalRead(pin_led);
  digitalWrite(pin_led, setLedState);

  return setLedState;
}

/**
 * Send Serial message back to server of current led state
 */
void sendLedStatus(int ledState) {
  String serialMessage;

  if (ledState == 1) {
    serialMessage = "led_status: true";
  } else if (ledState == 0) {
    serialMessage = "led_status: false";
  } else {
    serialMessage = "led_status: ERROR";
  }

  // Send server led status
  serverSerial.println(serialMessage);

  // Log led status to local terminal
  Serial.println(serialMessage);
}

int waterLevel() {
  int waterSensorHighStatus;
  int waterSensorMidStatus;
  int waterSensorLowStatus;
  int waterLevelSum;
  
  waterSensorLowStatus = digitalRead(waterSensorLowPin);
  waterSensorMidStatus = digitalRead(waterSensorMidPin);
  waterSensorHighStatus = digitalRead(waterSensorHighPin);

  // Calculte water level -> 0: empty - 3: full
  waterLevelSum = waterSensorLowStatus + waterSensorMidStatus + waterSensorHighStatus;

  return waterLevelSum;
}

void sendWaterLevelStatus(int waterLevel) {
  String waterLevelMessage;

  if (waterLevel == 0) {
    waterLevelMessage = "water_level: empty";
  } else if (ledState == 1) {
    waterLevelMessage = "water_level: low";
  } else if (ledState == 2) {{
    waterLevelMessage = "water_level: midway";
  } else if (ledState == 3) {{
    waterLevelMessage = "water_level: full";
  } else {
    waterLevelMessage = "water_level: ERROR";
  }

  // Send server led status
  serverSerial.println(waterLevelMessage);

  // Log water level to local terminal
  Serial.println(waterLevelMessage);
}
