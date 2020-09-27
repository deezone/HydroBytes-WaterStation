#include <SoftwareSerial.h>
SoftwareSerial serverSerial(10, 11);

// initialize pins
uint8_t pin_led = 12;

int waterSensorLowPin  = 6;
uint8_t waterSensorLowLedPin  = 2;

int waterSensorMidPin  = 7;
uint8_t waterSensorMidLedPin  = 3;

int waterSensorHighPin = 8;
uint8_t waterSensorHighLedPin  = 4;

/**
 * setup()
 */
void setup() {
  Serial.begin(9600);
  Serial.println("Sensors: serial connection started...");
  Serial.println("");

  Serial.println("HydroBites Water Station");
  Serial.println("v0.1.1");
  Serial.println("");

  Serial.println("Sensors: Sensors <-> Server serial connection started...");
  serverSerial.begin(9600);
  
  Serial.println("Sensors: setup complete...");

    // Assign pins
  pinMode(pin_led, OUTPUT);

  pinMode(waterSensorHighPin, INPUT);
  pinMode(waterSensorLowLedPin, OUTPUT);

  pinMode(waterSensorMidPin, INPUT);
  pinMode(waterSensorMidLedPin, OUTPUT);

  pinMode(waterSensorLowPin, INPUT);
  pinMode(waterSensorHighLedPin, OUTPUT);

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

      int ledState = getLedStatus();

      // Send Serial message back to server to confirm receipt
      serverSerial.println((String)"serial_status: true, led_status: " + ledState);

      // Log to serial monitor
      Serial.println("serial_status: true");
      Serial.println((String)"led_status: " + ledState);

    } else if (serverMessage.indexOf("GET /led/toggle") >= 0) {
      int ledState = toggleLed();
      sendLedStatus(ledState);
    } else if (serverMessage.indexOf("GET /led") >= 0) {
      int ledState = getLedStatus();
      sendLedStatus(ledState);
    } else if (serverMessage.indexOf("GET /water") >= 0) {
      int waterLevel = getWaterLevel();
      sendWaterLevelStatus(waterLevel);
    }
  }
}

/**
 * Get the current state of the LED - on (1)/off (0)
 */
int getLedStatus() {
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

  switch (ledState) {
    case 0:
      serialMessage = "led_status: false";
      break;

    case 1:
      serialMessage = "led_status: true";
      break;

    default:
      serialMessage = "led_status: ERROR";
  }

  // Send server led status
  serverSerial.println(serialMessage);

  // Log led status to local terminal
  Serial.println(serialMessage);
}

int getWaterLevel() {
  int waterSensorHighStatus = 0;
  int waterSensorMidStatus  = 0;
  int waterSensorLowStatus  = 0;
  int waterLevelSum = 0;

  // Digital: 1/0
  waterSensorLowStatus  = digitalRead(waterSensorLowPin);
  waterSensorMidStatus  = digitalRead(waterSensorMidPin);
  waterSensorHighStatus = digitalRead(waterSensorHighPin);

  // Set related led to the same on/off state of the water sensor
  digitalWrite(waterSensorLowLedPin, waterSensorLowStatus);
  digitalWrite(waterSensorMidLedPin, waterSensorMidStatus);
  digitalWrite(waterSensorHighLedPin, waterSensorHighStatus);

  // Calculate water level -> 0: empty - 3: full
  waterLevelSum = waterSensorLowStatus + waterSensorMidStatus + waterSensorHighStatus;

  return waterLevelSum;
}

void sendWaterLevelStatus(int waterLevel) {
  String waterLevelMessage;

  switch (waterLevel) {
    case 0:
      waterLevelMessage = "water_level: empty";
      break;

    case 1:
      waterLevelMessage = "water_level: low";
      break;

    case 2:
      waterLevelMessage = "water_level: midway";
      break;

    case 3:
      waterLevelMessage = "water_level: full";
      break;

    default:
      waterLevelMessage = "water_level: ERROR";
  }

  // Send server led status
  serverSerial.println(waterLevelMessage);

  // Log water level to local terminal
  Serial.println(waterLevelMessage);
}
