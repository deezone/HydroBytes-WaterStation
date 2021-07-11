/**
 * HydroBites WaterStation
 */

  /**
  * See https://www.deviceplus.com/arduino/arduino-preprocessor-directives-tutorial/
  * for details on how #include works. Specifically the search rules based on "<" and ">"
  * usage for where the preprocessor searches for the file.
  */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// CONNECTIONS:
// DS3231 SDA --> SDA
// DS3231 SCL --> SCL
// DS3231 VCC --> 3.3v or 5v
// DS3231 GND --> GND
#include <Wire.h>
#include <RtcDS3231.h>
#include <time.h>


const char *VERSION = "v0.4.0";
const char *RESEASE = "19 June 2021";

/**
 * NodeMCU pins
 * https://roboindia.com/tutorials/wp-content/uploads/2020/07/nodemcu_pins.png
 * D8: 15
 * D7: 13
 * D6: 12
 * D5: 14
 * D4: 2
 * D3: 0
 * D2: 4
 * D1: 5
 **/
const uint8_t  INTERNAL_LED = D4;
const uint8_t  EXTERNAL_LED = D7;

// uintxx_t: 8 bit: 0-255
const uint16_t PORT = 7000;
const uint8_t  RESPONSE_OK            = 200;
const uint16_t RESPONSE_BAD_REQUEST = 400;
const uint16_t RESPONSE_NOT_FOUND     = 404;
const uint16_t RESPONSE_ERROR         = 500;
const uint16_t RESPONSE_UNAVAILABLE   = 503;

const char* RESPONSE_OK_MESSAGE = "OK";
const char* RESPONSE_ERROR_MESSAGE = "Error";
const char* RESPONSE_NOT_FOUND_MESSAGE = "URL not found";

const char* REQUEST_ON = "on";
const char* REQUEST_OFF = "off";

const char* RESPONSE_ON_MESSAGE = "on";
const char* RESPONSE_OFF_MESSAGE = "off";

// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server(PORT);

// ESP8266 Static IP Address arduino Example
// https://circuits4you.com/2018/03/09/esp8266-static-ip-address-arduino-example/
// ESP8266 NodeMCU Static/Fixed IP Address (Arduino IDE)
// https://randomnerdtutorials.com/esp8266-nodemcu-static-fixed-ip-address-arduino/
// Static IP address
IPAddress local_IP(192, 168, 1, 200);

// Gateway IP address
IPAddress gateway(192, 168, 1, 1);

// Subnet
IPAddress subnet(255, 255, 0, 0);

// Create instance of Real-Time Clock (RTC)
RtcDS3231<TwoWire> Rtc(Wire);

const uint8_t waterPumpRelayPin  = 0; // D3
const uint8_t waterValveRelayPin = 2; // D4

const char *STATUS_OK = "OK";
const char *STATUS_ERROR = "Error";
const char *STATUS_ON = "On";
const char *STATUS_OFF = "Off";

void setup() {

  // Connect to local serial port for logging
  Serial.begin(115200);
  
  Serial.println("HydroBites Water Station");
  Serial.println(VERSION);
  Serial.println(RESEASE);
  Serial.println("");

  // Connect to internal LED and set to off
  pinMode(INTERNAL_LED, OUTPUT);
  digitalWrite(INTERNAL_LED, HIGH);

  // Connect to external LED and set to off
  pinMode(EXTERNAL_LED, OUTPUT);
  digitalWrite(EXTERNAL_LED, LOW);

  // Relays
  pinMode(waterPumpRelayPin, OUTPUT);
  pinMode(waterValveRelayPin, OUTPUT);

   // Initialize the rtc (real-time clock) object
  Rtc.Begin();

  // Record current system time as compile time
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  if (!Rtc.IsDateTimeValid()) {
    Serial.print(F("WARNING: RTC Invalid Date"));
          
    if (Rtc.LastError() != 0) {

      // communications error
      // see https://www.arduino.cc/en/Reference/WireEndTransmission for explination of number values
      Serial.print("RTC communications error = ");
      Serial.println(Rtc.LastError());

    } else {
      
      // Common Causes:
      //   1) first time you ran and the device wasn't running yet
      //   2) the battery on the device is low or even missing
      Serial.println(F("RTC - lost confidence in the DateTime!"));

      // set the RTC to the date and time this sketch was compiled
      // - also resets valid flag internally unless the Rtc device is having an issue
      Rtc.SetDateTime(compiled);
      Serial.println(F("RTC set to compile time"));
    }
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println(F("RTC was not actively running, starting now"));
    Rtc.SetIsRunning(true);
  }

  // RTC setting check
  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
    Serial.println(F("RTC is older than compile time!  (Updating DateTime)"));
    Rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println(F("RTC is newer than compile time. (this is expected)"));
  } else if (now == compiled) {
    Serial.println(F("RTC is the same as compile time! (not expected but all is good... what are the chances!?!)"));
  }

  // Do not assume the Rtc was last configured correctly
  // Clear RTC setting to needed state
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 

  Serial.println("Real-Time Clock: Initialized...");
  Serial.print(pRtcDateTime(compiled));
  Serial.println();

  // @TO-DO: move to secrets file
  // WiFi.begin("HydroBytes-LR", "Z!ppyD00+hydrobytes");
  WiFi.begin("KyKyRyKyKyy", "pond482dan446twin");

  // Configures static IP address
  // https://randomnerdtutorials.com/esp8266-nodemcu-static-fixed-ip-address-arduino/
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println(F("STA Failed to configure"));
  }

  Serial.print(F("WiFi connecting "));
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(PORT);

  // Server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, getSystemStatus);
  server.on("/led", HTTP_POST, postLed);
  server.on("/time", HTTP_GET, getRTCTime);
//  server.on("/time", HTTP_POST, postRTCTime);
//  server.on("/water", HTTP_GET, getWaterStatus);
//  server.on("/water/fill", HTTP_POST, getWater);
//  server.on("/water/irrigate", HTTP_POST, postWater);
  server.onNotFound(handleNotFound);

  // Start the server
  server.begin();

  Serial.println(F("See GET / for availble routes"));
  Serial.println();
  
  Serial.println(F("Let's ROCK!"));
}

void loop() {
  
  // Process http requests
  server.handleClient();
}

/**
 * Controllers
 **/

/**
 * handleRoot
 * A directory of the available endpoints
 **/
void handleRoot(void) {
  const uint16_t docSize = 512;

  String buf((char *)0);
  buf.reserve(docSize);
  StaticJsonDocument<docSize> doc;

  // Log request to serial channel
  Serial.println(F("GET /"));

  // Compose response
  doc["hydrobytes-water-station"] = RESPONSE_OK_MESSAGE;
  doc["status"]["code"] = RESPONSE_OK;

  doc["release"]["version"] = VERSION;
  doc["release"]["date"] = RESEASE;

  doc["paths"]["/"] = "Welcome";
  doc["paths"]["/status"] = "GET - Current health of subsystems";
  doc["paths"]["/led"] = "POST - Change the state of LED - on/off";
  doc["paths"]["/time"] = "GET - Get the current time from the Real-Time Clock (RTC)";
//  doc["paths"]["/time"] = "POST - Set the time on the RTC";
//  doc["paths"]["/water"] = "GET - Get the water levels in the water barrel";
//  doc["paths"]["/water/fill"] = "POST - Fill the water barrel to at least a certain level";
//  doc["paths"]["/water/irrigate"] = "POST - Pump water for a time period, volume of water or both";

  serializeJson(doc, buf);

  server.send(RESPONSE_OK, F("application/json"), buf);
}

/**
 * Status request response. Returns http response to server http request based on
 * status reponse from sensor controller.
 *
 * Go to http://192.168.4.1/status
 */
void getSystemStatus(void) {
  const uint16_t docSize = 256;

//  char clockStatus[]  = STATUS_ERROR; 
//  char waterSensorStatus[] = STATUS_ERROR;
//  char waterSensorLowStatus[] = STATUS_ERROR;
//  char waterSensorMidStatus[] = STATUS_ERROR;
//  char waterSensorHighStatus[] = STATUS_ERROR;
//  char waterPumpStatus[] = STATUS_ERROR;
//  char waterValveStatus[] = STATUS_ERROR;

  String buf((char *)0);
  buf.reserve(docSize);
  StaticJsonDocument<docSize> doc;

  // Log request to serial channel
  Serial.println(F("GET /status"));

  uint8_t responseCode = RESPONSE_OK;

  // getWaterSensorStatus();

  doc["hydrobytes-water-station"] = RESPONSE_OK_MESSAGE;
  doc["release"]["version"] = VERSION;
  doc["release"]["date"] = RESEASE;
  
  doc["status"]["code"] = RESPONSE_OK;

  doc["status"]["led"]["internal"] = getPinStatus(INTERNAL_LED) ? RESPONSE_ON_MESSAGE : RESPONSE_OFF_MESSAGE;
  doc["status"]["led"]["external"] = getPinStatus(EXTERNAL_LED) ? RESPONSE_OFF_MESSAGE : RESPONSE_ON_MESSAGE;

  doc["status"]["real-time-clock"]["date-time"] = pRtcDateTime(Rtc.GetDateTime());
  doc["status"]["real-time-clock"]["timestamp"] = pRtcTimestamp(Rtc.GetDateTime());
  doc["status"]["real-time-clock"]["tempature"] = pRtcTemp(Rtc.GetTemperature());

//  doc["status"]["water-sensors"] = waterSensorStatus;
//  doc["status"]["water-sensors"]["low"] = waterSensorLowStatus;
//  doc["status"]["water-sensors"]["mid"] = waterSensorMidStatus;
//  doc["status"]["water-sensors"]["high"] = waterSensorHighStatus;
//  doc["status"]["water-pump"] =  waterPumpStatus;
//  doc["status"]["water-valve"] =  waterValveStatus;

  serializeJson(doc, buf);

  server.send(responseCode, F("application/json"), buf);
}

/**
   Get the current state of the LED - on(1) / off(0)
*/
void postLed(void) {
  const uint16_t docSize = 256;

  String buf((char *)0);
  buf.reserve(docSize);
  StaticJsonDocument<docSize> postDoc;
  StaticJsonDocument<docSize> doc;

  // Log request to serial channel
  Serial.println(F("POST /led"));

  // Validate POST request has at least one required value
  if(!server.hasArg("plain") || server.arg("plain") == NULL) {

    String errorMsg((char *)0);
    errorMsg = "Not Found: POST is missing body";

    Serial.println(errorMsg);

    doc["hydrobytes-water-station"] = RESPONSE_ERROR_MESSAGE;
    doc["status"]["code"] = RESPONSE_NOT_FOUND;
    doc["status"]["message"] = errorMsg;

    serializeJson(doc, buf);

    server.send(RESPONSE_NOT_FOUND, F("application/json"), buf);

    return;
  }

  String postBody = server.arg("plain");
  DeserializationError deserializeError = deserializeJson(postDoc, postBody);

  if (deserializeError) {

    String errorMsg((char *)0);
    errorMsg = "Error in parsing json body: ";
    errorMsg += deserializeError.c_str();

    Serial.println(errorMsg);

    doc["hydrobytes-water-station"] = RESPONSE_ERROR_MESSAGE;
    doc["status"]["code"] = RESPONSE_ERROR;
    doc["status"]["message"] = errorMsg;

    serializeJson(doc, buf);
    server.send(RESPONSE_ERROR, F("application/json"), buf);

    return;
  }

  JsonObject postObj = postDoc.as<JsonObject>();
      
  if (!postObj.containsKey("internal") && !postObj.containsKey("external")) {

    String errorMsg((char *)0);
    errorMsg = "Bad Request: Invalid body value, 'internal' and/or 'external' value missing";

    Serial.println(errorMsg);
 
    doc["hydrobytes-water-station"] = RESPONSE_ERROR_MESSAGE;
    doc["status"]["code"] = RESPONSE_BAD_REQUEST;
    doc["status"]["message"] = errorMsg;

    serializeJson(doc, buf);
    server.send(RESPONSE_BAD_REQUEST, F("application/json"), buf);

    return;
  }

  
  /**
   * Process valid POST
   */
  if (postObj["internal"] == REQUEST_ON) {

    Serial.println("Internal LED enabled");
    digitalWrite(INTERNAL_LED, LOW);

  } else if (postObj["internal"] == REQUEST_OFF) {

    Serial.println("Internal LED disabled");
    digitalWrite(INTERNAL_LED, HIGH);

  }

  if (postObj["external"] == REQUEST_ON) {

    Serial.println("External LED enabled");
    digitalWrite(EXTERNAL_LED, HIGH);

  } else if (postObj["external"] == REQUEST_OFF) {

    Serial.println("External LED disabled");
    digitalWrite(EXTERNAL_LED, LOW);

  }

  // Compose response
  doc["hydrobytes-water-station"] = RESPONSE_OK_MESSAGE;
  doc["status"]["code"] = RESPONSE_OK;
  doc["status"]["led"]["internal"] = getPinStatus(INTERNAL_LED) ? RESPONSE_ON_MESSAGE : RESPONSE_OFF_MESSAGE;
  doc["status"]["led"]["external"] = getPinStatus(EXTERNAL_LED) ? RESPONSE_OFF_MESSAGE : RESPONSE_ON_MESSAGE;

  serializeJson(doc, buf);
  server.send(RESPONSE_OK, F("application/json"), buf);

}

/**
 * Read digital pin status
 */
bool getPinStatus(int pin) {
  bool pinStatus = false;

  if (digitalRead(pin) == LOW) {
    pinStatus = true;
  }

  return pinStatus;
}

void getRTCTime() {
  const uint16_t docSize = 512;

  String buf((char *)0);
  buf.reserve(docSize);
  StaticJsonDocument<docSize> doc;

  // Log request to serial channel
  Serial.println("GET /time");

  if (!Rtc.IsDateTimeValid()) {
    
    if (Rtc.LastError() != 0) {
      
      // Communications error
      // See https://www.arduino.cc/en/Reference/WireEndTransmission for error number definitions
      Serial.print("RTC communications error: ");
      Serial.println(Rtc.LastError());
      
    } else {
      // Common Causes
      //  1) the battery on the device is low or even missing and the power line was disconnected
      Serial.println("RTC lost confidence in the DateTime!");
    }

  }

  RtcDateTime nowRtc = Rtc.GetDateTime();
  RtcTemperature tempRtc = Rtc.GetTemperature();

  // Compose response
  doc["hydrobytes-water-station"] = "ok";
  doc["clock"]["time"] = pRtcDateTime(nowRtc);
  doc["clock"]["timestamp"] = pRtcTimestamp(nowRtc);
  doc["clock"]["temperature"] = pRtcTemp(tempRtc);

  serializeJson(doc, buf);
  server.send(RESPONSE_OK, F("application/json"), buf);
}

///**
// * Water level response
// */
//void getWaterLevel() {
//  String waterLevelStatus;
//  String pumpStatus;
//  String toggleSinceDuration;
//
//  String buf;
//  DynamicJsonDocument doc(128);
//  String responseBody;
//  uint8_t responseCode;
//
//  // wl: x
//  // water_level: x
//  waterLevelStatus = sensorMessage.substring(4, 5);
//  waterLevelStatus.trim();
//  waterLevelStatus = getWaterLevelMessage(waterLevelStatus);
//
//  pumpStatus = sensorMessage.substring(11, 12);
//  pumpStatus.trim();
//  pumpStatus = pumpStatus == "1" ? "on" : "off";
//
//  // xxx:xx:xx:xx
//  toggleSinceDuration = sensorMessage.substring(18, 30);
//  toggleSinceDuration.trim();
//  toggleSinceDuration = toggleSinceDuration.toInt() >= 0 ? toggleSinceDuration : "ERROR";
//
//  // Compose response
//  Serial.println("water_level: ");
//  if (waterLevelStatus == "empty" || waterLevelStatus == "low" || waterLevelStatus == "mid" || waterLevelStatus == "full") {
//    Serial.println(response_OK);
//    responseCode = response_OK;
//
//    doc["water"]["level"] = waterLevelStatus;
//    doc["water"]["irrigation"]["status"] = pumpStatus;
//    doc["water"]["irrigation"]["since"] = toggleSinceDuration;
//
//  } else {
//    Serial.println(response_UNAVAILABLE);
//    responseCode = response_UNAVAILABLE;
//
//    doc["water"]["status"] = "ERROR";
//  }
//
//  serializeJson(doc, buf);
//  server.send(responseCode, F("application/json"), buf);
//}

/**
 * Send HTTP status 404 (Not Found) when there's no handler for the URI
 * in the request
 */
void handleNotFound() {
  
  String buf;
  DynamicJsonDocument doc(128);

  doc["status"]["code"] = RESPONSE_NOT_FOUND;
  doc["status"]["message"] = RESPONSE_NOT_FOUND_MESSAGE;

  serializeJson(doc, buf);
  server.send(RESPONSE_NOT_FOUND , "application/json", buf);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

/**
 * https://forum.arduino.cc/t/can-a-function-return-a-char-array/63405/5
 */
char* pRtcDateTime(const RtcDateTime& dt) {

  char * buf = (char *) malloc (20);
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

    strcpy(buf, datestring);

    return buf;
}

/**
 * 
 */
char* pRtcTimestamp(const RtcDateTime& dt) {

  char * buf = (char *) malloc (12);
  char timestamp[12];
    
  // Calculate timestamp value for current time
  struct tm ltm = {0};
  ltm.tm_year = dt.Year() - 1900;
  ltm.tm_hour = dt.Hour();
  ltm.tm_min = dt.Minute();
  ltm.tm_sec = dt.Second();

  double epoch_seconds = difftime(mktime(&ltm), (time_t)0);
  dtostrf(epoch_seconds, 10, 0, timestamp);

  strcpy(buf, timestamp);

  return buf;
}

/**
 * 
 */
char* pRtcTempC(const RtcTemperature& t) {
  
  char * buf = (char *) malloc (8);
  char temp[8];
  float tempFloat;
  
  tempFloat = t.AsFloatDegC();
  temp = dtostrf(temp);

  strcpy(buf, temp);

  return temp;
}
