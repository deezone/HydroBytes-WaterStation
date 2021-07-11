/**
 * HydroBites WaterStation sensors
 * 
 * DS3231
 * https://github.com/Makuna/Rtc
 * 
 */
#include <SoftwareSerial.h>

/* for software wire use below
#include <SoftwareWire.h>  // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
SoftwareWire myWire(SDA, SCL);
RtcDS3231<SoftwareWire> Rtc(myWire);
 for software wire use above */

/* for normal hardware wire use below */
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);

// Time to track time between when water pump is tunnered on/off
unsigned long previousIrrigationTime = Rtc.GetDateTime();
>>>>>>> Stashed changes

const String server_VERSION = "v0.2.1";
const String server_RELEASE = "25 October 2020";

// initialize pins
const uint8_t waterSensorLowLedPin  = 2;
const uint8_t waterSensorMidLedPin  = 3;
const uint8_t waterSensorHighLedPin = 4;

const uint8_t waterPumpRelayPin = 5;

const int waterSensorLowPin  = 6;
const int waterSensorMidPin  = 7;
const int waterSensorHighPin = 8;

// used by ESP8266 for serial communication
SoftwareSerial serverSerial(10, 11); // Rx, Tx

const uint8_t ledPin = 12;

const uint8_t waterLevel_empty = 0;
const uint8_t waterLevel_low   = 1;
const uint8_t waterLevel_mid   = 2;
const uint8_t waterLevel_high  = 3;

/**
   setup()
*/
void setup() {

  Serial.begin(9600);
  Serial.println("Sensors: serial connection started...");
  Serial.println("");

  Serial.println("HydroBites Water Station");
  Serial.println(server_VERSION);
  Serial.println(server_RELEASE);
  Serial.println("");

  Serial.println("Sensors: Sensors <-> Server serial connection started...");
  serverSerial.begin(9600);

  Serial.println("Sensors: setup complete...");

  // Assign pins
  // Water sensors
  // pinMode(waterSensorLowPin, INPUT);
  // pinMode(waterSensorMidLedPin, OUTPUT);
  // pinMode(waterSensorHighPin, INPUT);

  // Water pump
  // pinMode(waterPumpRelayPin, OUTPUT);

  // Water sensor level LED indicators
  // pinMode(waterSensorLowLedPin, OUTPUT);
  // pinMode(waterSensorMidPin, INPUT);
  // pinMode(waterSensorHighLedPin, OUTPUT);

  // Test LED
  // pinMode(ledPin, OUTPUT);

  Serial.println("Sensors: pin assignment complete...");

  Serial.println("Sensors: pin assignment complete...");
  Serial.println("");

  // Initialize the rtc (real-time clock) object
  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid()) {
    Serial.print("WARNING: RTC Invalid Date");
          
    if (Rtc.LastError() != 0) {

      // communications error
      // see https://www.arduino.cc/en/Reference/WireEndTransmission for explination of number values
      Serial.print("RTC communications error = ");
      Serial.println(Rtc.LastError());

    } else {
      
      // Common Causes:
      //   1) first time you ran and the device wasn't running yet
      //   2) the battery on the device is low or even missing
      Serial.println("RTC - lost confidence in the DateTime!");

      // set the RTC to the date and time this sketch was compiled
      // - also resets valid flag internally unless the Rtc device is having an issue
      Rtc.SetDateTime(compiled);
      Serial.println("RTC set to compile time");
    }
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }


  // RTC setting check
  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compiled) {
    Serial.println("RTC is the same as compile time! (not expected but all is good... what are the chances!?!)");
  }

  // Do not assume the Rtc was last configured correctly
  // Clear RTC setting to needed state
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 

  Serial.println("Real-Time Clock: Initialized...");
  Serial.println();

  /**
     Serial.write are transmitted in the background (from an interrupt handler) allowing your
     sketch code to immediately resume processing. This is usually a good thing (it can make the
     sketch more responsive) but sometimes you want to wait until all characters are sent. You can
     achieve this by calling Serial.flush() immediately following Serial.write().
  */
  Serial.flush();
}

/**
   loop() - event loop
*/
void loop() {
  String serverMessage;
  int irrigationDuration = 0;

  if (!Rtc.IsDateTimeValid()) {
    
    if (Rtc.LastError() != 0) {
      
      // we have a communications error
      // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
      // what the number means
      Serial.print("RTC communications error = ");
      Serial.println(Rtc.LastError());
      
    } else {
      // Common Causes
      //  1) the battery on the device is low or even missing and the power line was disconnected
      Serial.println("RTC lost confidence in the DateTime!");
    }

  }

  RtcDateTime now = Rtc.GetDateTime();
  Serial.print("RTC time: ");
  printDateTime(now);
  Serial.println();

  RtcTemperature temp = Rtc.GetTemperature();
  Serial.print("RTC temperature: ");

  // you may also get the temperature as a float and print it
  // Serial.print(temp.AsFloatDegC());
  temp.Print(Serial);
  Serial.println("C");

  while (serverSerial.available()) {
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
      unsigned long irrigationDuration = getPumpToggleDuration();

      sendWaterStatus(waterLevelState, waterPumpState, irrigationDuration);

    } else if (serverMessage.indexOf("GET /water") >= 0) {

      int waterLevelState = getWaterLevel();
      int waterPumpState =  getPumpStatus();
      unsigned long irrigationToggleDuration = getPumpToggleDuration();

      sendWaterStatus(waterLevelState, waterPumpState, irrigationToggleDuration);

    }
  }
}

/**
   Report current serial and sensor status
*/
void sendStatus() {
  int sensorState = 0; // false

  int waterLevelState = getWaterLevel();
  if (waterLevelState > 0) {
    sensorState = 1; // true
  }

  int rtcState = Rtc.GetDateTime();
  if (rtcState > 0) {
    clockState = 1; // true
  }

  // Send Serial message back to server to confirm receipt. If this response works then
  // the serial connection is working
  // serial_status (sr), sensor_status (ss)
  serverSerial.print("sr: 1, ss: ");
  serverSerial.println(sensorState);

  // Log to serial monitor
  Serial.println("serial_status: true, sensor_status: " + sensorState);
}

/**
 * Get the current state of the LED - on(1) / off(0)
 */
int getLedStatus() {
  return digitalRead(ledPin);
}

/**
   Adjust the state of the LED between on/off
*/
int toggleLed() {
  int setLedState = 0;

  // Write the opposite of the current state
  setLedState = !digitalRead(ledPin);
  digitalWrite(ledPin, setLedState);

  Serial.print("led_state: ");
  Serial.println(setLedState);

  return setLedState;
}

/**
   Send Serial message back to server of current led state
*/
void sendLedStatus(int ledState) {
  // Send server led status
  serverSerial.print("ls: ");
  serverSerial.println(ledState);

  // Log led status to local terminal
  Serial.print("led_state: ");
  Serial.println(ledState);
}

/**
   Read water level sensors and toggle related LED
*/
int getWaterLevel() {
  int waterSensorHighStatus = 0;
  int waterSensorMidStatus  = 0;
  int waterSensorLowStatus  = 0;
  bool sensorCheckOk = false;
  int waterLevel = 0;

  // Digital: 1/0
  waterSensorLowStatus  = digitalRead(waterSensorLowPin);
  waterSensorMidStatus  = digitalRead(waterSensorMidPin);
  waterSensorHighStatus = digitalRead(waterSensorHighPin);

  // Set related led to the same on/off state of the water sensor
  digitalWrite(waterSensorLowLedPin, waterSensorLowStatus);
  digitalWrite(waterSensorMidLedPin, waterSensorMidStatus);
  digitalWrite(waterSensorHighLedPin, waterSensorHighStatus);

  // Validate sensor readings, a lower sensor should not have an off status
  // High
  if (waterSensorHighStatus == 1 && waterSensorMidStatus == 1 && waterSensorLowStatus == 1) {
    return 3;
  }

  // Mid
  if (waterSensorHighStatus == 0 && waterSensorMidStatus == 1 && waterSensorLowStatus == 1) {
    return 2;
  }

  // Low
  if (waterSensorHighStatus == 0 && waterSensorMidStatus == 0 && waterSensorLowStatus == 1) {
    return 1;
  }

  // Empty
  if (waterSensorHighStatus == 0 && waterSensorMidStatus == 0 && waterSensorLowStatus == 0) {
    return 0;
  }

  // Error
  return -1;
}

/**
   Adjust the state of water pump between on/off
*/
int toggleWaterPump(int waterLevel) {
  int setPumpState = 0;

  // Error detecting water level
  if (waterLevel == -1) {
    return -1;
  }

  // Do not run pump if water level is low
  if (waterLevel <= waterLevel_mid) {
    return 0;
  }

  // Write the opposite of the current state
  // The relay is active low, it turns on when set to LOW
  setPumpState = !digitalRead(waterPumpRelayPin);

  digitalWrite(waterPumpRelayPin, setPumpState);

  // pump state toggled, set new irrigation time
  previousIrrigationTime = Rtc.GetDateTime();

  Serial.print("water_pump: ");
  Serial.println(setPumpState);

  return setPumpState;
}

/**
   Get the current state of the pump - on (1)/off (0)
*/
int getPumpStatus() {
  return digitalRead(waterPumpRelayPin);
}

/**
   Get the amount of time since the pump was last toggled
*/
unsigned long getPumpToggleDuration() {
  return millis() - previousIrrigationTime;
}

/**
 * Send Serial message back to server of current irrigation (water pump) state
 *
 * wl: x, is: x, id: x
 */
void sendWaterStatus(int waterLevelState, int pumpState, unsigned long irrigationDuration) {
  int seconds = (int) ((irrigationDuration / 1000) % 60);
  int minutes = (int) (((irrigationDuration / 1000) / 60) % 60);
  int hours = (int) ((((irrigationDuration / 1000) / 60) / 24) % 24);
  return Rtc.GetDateTime() - previousIrrigationTime;
  
}

/**
   Send Serial message back to server of current irrigation (water pump) state

   wl: x, ps: x
*/
void sendWaterStatus(int waterLevelState, int pumpState) {

  // Send water level state
  serverSerial.print("wl: ");
  serverSerial.print(waterLevelState);

  // Send pump status
  serverSerial.print(", is: ");
  serverSerial.print(pumpState);

  // Send irrigation duration
  serverSerial.print(", id: ");

  char serverbuf[9];
  sprintf(serverbuf, "%02d:%02d:%02d", hours, minutes, seconds);
  serverSerial.println(serverbuf);

  // Log water level state to local terminal
  Serial.print("water_level: ");
  Serial.println(waterLevelState);

  // Log pump status to local terminal
  Serial.print("irrigation_status: ");
  Serial.println(pumpState);
}

/**
   Send Serial message back to server of irrigation (water pump) running duration since last toggle

   days:hours:minutes:seconds
   id: xxx:xx:xx:xx
*/
void sendIrrigationDuration(unsigned long irrigationDuration) {
  int seconds = (int) ((irrigationDuration / 1000) % 60);
  int minutes = (int) (((irrigationDuration / 1000) / 60) % 60);
  int hours = (int) ((((irrigationDuration / 1000) / 60) / 24) % 24);
  int days = round(((irrigationDuration / 1000) / 60) / 24);

  // Send irrigation duration
  serverSerial.print(", id: ");

  char irrigationDurationBuf[13];
  sprintf(irrigationDurationBuf, "%03d:%02d:%02d:%02d", days, hours, minutes, seconds);
  serverSerial.println(irrigationDurationBuf);

  // Log irrigation duration to local terminal
  Serial.print("irrigation_duration: ");

  char localbuf[9];
  sprintf(localbuf, "%02d:%02d:%02d", hours, minutes, seconds);
  Serial.println(localbuf);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt) {
  char datestring[20];

  snprintf_P(
    datestring, 
    countof(datestring),
    PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
    dt.Month(),
    dt.Day(),
    dt.Year(),
    dt.Hour(),
    dt.Minute(),
    dt.Second()
  );
  
  Serial.print(datestring);
}
