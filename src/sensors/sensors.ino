#include <SoftwareSerial.h>
SoftwareSerial serverSerial(10, 11);

// initialize pins
const uint8_t waterSensorLowLedPin  = 2;
const uint8_t waterSensorMidLedPin  = 3;
const uint8_t waterSensorHighLedPin = 4;

const uint8_t waterPumpRelayPin = 5;

const int waterSensorLowPin  = 6;
const int waterSensorMidPin  = 7;
const int waterSensorHighPin = 8;

// Pins 10, 11 used by ESP8266

const uint8_t ledPin = 12;

const uint8_t waterLevel_empty = 0;
const uint8_t waterLevel_low   = 1;
const uint8_t waterLevel_mid   = 2;
const uint8_t waterLevel_high  = 3;

/**
 * setup()
 */
void setup() {
  Serial.begin(9600);
  Serial.println("Sensors: serial connection started...");
  Serial.println("");

  Serial.println("HydroBites Water Station");
  Serial.println("v0.2.0");
  Serial.println("");

  Serial.println("Sensors: Sensors <-> Server serial connection started...");
  serverSerial.begin(9600);
  
  Serial.println("Sensors: setup complete...");

  // Assign pins
  // Water sensors
  pinMode(waterSensorLowPin, INPUT);
  pinMode(waterSensorMidLedPin, OUTPUT);
  pinMode(waterSensorHighPin, INPUT);

  // Water pump
  pinMode(waterPumpRelayPin, OUTPUT);

  // Water sensor level LED indicators
  pinMode(waterSensorLowLedPin, OUTPUT);
  pinMode(waterSensorMidPin, INPUT);
  pinMode(waterSensorHighLedPin, OUTPUT);

  // Test LED
  pinMode(ledPin, OUTPUT);

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
      sendStatus();
    } else if (serverMessage.indexOf("GET /led/toggle") >= 0) {
      int ledState = toggleLed();
      sendLedStatus(ledState);
    } else if (serverMessage.indexOf("GET /led") >= 0) {
      int ledState = getLedStatus();
      sendLedStatus(ledState);
    } else if (serverMessage.indexOf("GET /water/irrigate") >= 0) {
      int waterLevelState = getWaterLevel();
      int waterPumpState = toggleWaterPump(waterLevelState);
      sendWaterPumpStatus(waterPumpState, waterLevelState);
    } else if (serverMessage.indexOf("GET /water") >= 0) {
      int waterLevelState = getWaterLevel();
      sendWaterLevelStatus(waterLevelState);
    }
  }
}

/**
 * Report current serial and sensor status
 */
void sendStatus() {
  String sensorState = "false";

  int waterLevelState = getWaterLevel();
  if (waterLevelState > 0) {
    sensorState = "true";
  }

  // Send Serial message back to server to confirm receipt. If this response works then
  // the serial connection is working
  serverSerial.println("serial_status: true, sensor_status: " + sensorState);

  // Log to serial monitor
  Serial.println("serial_status: true");
  Serial.println("sensor_status: " + sensorState);
}

/**
 * Get the current state of the LED - on (1)/off (0)
 */
int getLedStatus() {
  int ledState = 0;

  // Get LED reading
  ledState = digitalRead(ledPin);

  return ledState;
}

/**
 * Adjust the state of the LED between on/off
 */
int toggleLed() {
  int setLedState = 0;

  // Write the opposite of the current state
  setLedState = !digitalRead(ledPin);
  digitalWrite(ledPin, setLedState);

  Serial.print("ledPin: ");
  Serial.println(setLedState);

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

/**
 * Read water level sensors and toggle related LED
 */
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

/**
 * Send serial message based on water level readings
 */
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

/**
 * Adjust the state of water pump between on/off
 */
int toggleWaterPump(int waterLevel) {
  int setPumpState = 0;
  int waterLevel = 0;

  // Do not run pump if water level is low
  if (waterLevel <= waterLevel_mid) {
    return LOW;
  }

  // Write the opposite of the current state
  // The relay is active low, it turns on when set to LOW
  setPumpState = !digitalRead(waterPumpRelayPin);

  digitalWrite(waterPumpRelayPin, setPumpState);

  Serial.print("waterPumpRelayPin: ");
  Serial.println(setPumpState);

  return setPumpState;
}

/**
 * Send Serial message back to server of current irrigation (water pump) state
 */
void sendWaterPumpStatus(int pumpState, int waterLevelState) {
  String serialMessage;

  switch (pumpState) {
    case 0:
      serialMessage = "irrigation_status: false";
      break;

    case 1:
      serialMessage = "irrigation_status: true";
      break;

    default:
      serialMessage = "irrigation_status: ERROR";
  }

  // Send server pump status
  serverSerial.println(serialMessage);

  // Log pump status to local terminal
  Serial.println(serialMessage);
}
