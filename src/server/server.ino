/**
 * HydroBites WaterStation http server
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

/**
 * WiFi credentials.
 *
 * Note use of "ifndef" to ensure excisting constant "APSSID" does not attempt to be ressigned.
 * If it's undefined, the next line defines the constant.
 *
 * https://www.arduino.cc/reference/en/language/structure/further-syntax/define/
 *
 * #define is a useful C++ component that allows the programmer to give a name to a constant value
 * before the program is compiled. Defined constants in arduino don’t take up any program memory
 * space on the chip. The compiler will replace references to these constants with the defined value
 * at compile time.
 */
#ifndef APSSID
#define APSSID "HydroBytes_WaterStation"
#define APPSK  "undefined" // password
#endif

/**
 * https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/const/
 *
 * In general, the const keyword is preferred for defining constants and should be used instead
 * of #define.
 */

const char *ssid = APSSID;
const char *password = APPSK;

const String server_VERSION = "v0.2.2";
const String server_RESEASE = "31 October 2020";

const uint8_t response_OK = 200;
const uint8_t response_NOT_FOUND = 404;
const uint8_t response_UNAVAILABLE = 503;

// Instantiate ESP8266WebServer class as "server"
ESP8266WebServer server(80);

/**
 * Setup
 */
void setup() {
  // Wait for serial port to be available
  // Include a delay in the routine. That would allow background tasks that need to run. You can not have long blocking
  // code with the ESP8266. It will cause the behavior you see:

  // Uno (sensors) - received from ESP-01 (server):
  // 21:40:37.136 ->  ets Jan  8 2013,rst cause:2, boot mode:(3,7)

  // ESP8266 12E keeps rebooting (rst cause:2, boot mode:(3,6))
  // https://github.com/esp8266/Arduino/issues/3241

  // rst cause:2
  // 0 -> normal startup by power on
  // 1 -> hardware watch dog reset
  // 2 -> software watch dog reset (From an exception)
  // 3 -> software watch dog reset system_restart (Possibly unfed watchdog got angry)
  // 4 -> soft restart (Possibly with a restart command)
  // 5 -> wake up from deep-sleep

  // Wait for sensor controller to start as serial monitor is typically connected there for debugging
  // delay() forces the logging to be in the expected order and also ensures timing of serial connection
  // happens in the expected order
  delay(10000);

  // Connect to local serial port. Used to send messages to sensor controller
  Serial.begin(9600);

  Serial.println("HydroBites Water Station - Server");
  Serial.println(server_VERSION);
  Serial.println(server_RESEASE);
  Serial.println("");

  Serial.println("Configuring access point...");

  /* Password parameter removed to make open network.
   * WiFi.softAP(ssid, password);
   */
  Serial.print("Setting soft-AP ... ");
  WiFi.softAP(ssid) ? "Ready" : "Failed!";
  Serial.println("");

  IPAddress stationIP = WiFi.softAPIP();
  Serial.println("AP (station) IP address: " + stationIP);

  Serial.println("Local IP address: " + WiFi.localIP());
  Serial.flush();

  // server routers
  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, requestStatus);
  server.on("/led", HTTP_GET, requestLedStatus);
  server.on("/led/toggle", HTTP_GET, requestLedToggle);
  server.on("/water", HTTP_GET, requestWaterLevel);
  server.on("/water/irrigate", HTTP_GET, requestIrrigateToggle);

  server.begin();
  Serial.println("HTTP server started");
  Serial.flush();
}

/**
 * Event loop
 */
void loop() {
  String sensorMessage;

  // Serial messages from sensors (Uno)
  while (Serial.available()) {
    sensorMessage = Serial.readStringUntil('\n');

    // serial_status
    if (sensorMessage.indexOf("ss:") >= 0) {
      responseStatus(sensorMessage);

    // led_status
    } else if (sensorMessage.indexOf("ls:") >= 0) {
      responseLedStatus(sensorMessage);

    // water_status (wl: x, ps: x, id: xx:xx:xx)
    } else if (sensorMessage.indexOf("id:") >= 0) {
      responseWaterLevel(sensorMessage);

    // irrigation_status (wl: x, ps: x)
    } else if (sensorMessage.indexOf("wl:") >= 0) {
      responseIrrigate(sensorMessage);
    }
  }

  // Process http requests
  server.handleClient();
}

/*
 * Controllers
 * 
 * handleRoot
 * Go to http://192.168.4.1 in web browser for http access to network
 */
void handleRoot() {
  String buf;
  DynamicJsonDocument doc(512);

  // Write to serial channel to inform Ardunio (Uno) of event
  Serial.println("GET /");

  // Compose response
  doc["hydrobytes-water-station"] = "ok";

  doc["release"]["version"] = server_VERSION;
  doc["release"]["date"] = server_RESEASE;

  doc["paths"]["/"] = "Welcome";
  doc["paths"]["/status"] = "Current health of subsystems";
  doc["paths"]["/led"] = "Current state of LED - on/off";
  doc["paths"]["/led/toggle"] = "Toggle LED on/off";
  doc["paths"]["/water"] = "Current water status";
  doc["paths"]["/water/irrigate"] = "Toggle irrigation on/off";

  serializeJson(doc, buf);

  server.send(response_OK, F("application/json"), buf);
}

/**
 * Status request controller
 */
void requestStatus() {
  Serial.println("GET /status");
}

/**
 * Status request response. Returns http response to server http request based on
 * status reponse from sensor controller.
 *
 * Go to http://192.168.4.1/status
 */
void responseStatus(String sensorMessage) {
  String serialStatus;
  String sensorStatus;
  String buf;
  DynamicJsonDocument doc(128);
  uint8_t responseCode = response_OK;

  // sr: x, ss: x
  // serial_status: x, sensor_status: x
  serialStatus = sensorMessage.substring(4, 5);
  serialStatus.trim();
  serialStatus = serialStatus == "1" ? "true" : "false";

  sensorStatus = sensorMessage.substring(11, 12);

  Serial.println("responseStatus() sensorStatus preTrim(): ->" + sensorStatus + "<-");
  sensorStatus.trim();
  sensorStatus = sensorStatus == "1" ? "true" : "false";

  // Compose response
  if (serialStatus != "true" || sensorStatus != "true") {
    responseCode = response_UNAVAILABLE;
  }

  doc["status"]["two-way-serial-communication"] = serialStatus;
  doc["status"]["water-sensors"] = sensorStatus;
  serializeJson(doc, buf);

  server.send(responseCode, F("application/json"), buf);
}

/**
 * LED status serial request to sensor micro controller
 */
void requestLedStatus() {
  Serial.println("GET /led");
}

/**
 * LED toggle serial request to sensor micro controller
 */
void requestLedToggle() {
  Serial.println("GET /led/toggle");
}

/**
 * Led status response
 *
 * Go to http://192.168.4.1/led
 */
void responseLedStatus(String sensorMessage) {
  String ledStatus;
  String responseBody;
  String buf;
  DynamicJsonDocument doc(64);
  uint8_t responseCode;

  // ls: x
  // led_status: x
  ledStatus = sensorMessage.substring(4, 5);
  ledStatus.trim();
  ledStatus = ledStatus == "1" ? "true" : "false";

  // Compose response
  if (ledStatus == "true" || ledStatus == "false") {
    Serial.print("led_status: ");
    Serial.println(response_OK);
    responseCode = response_OK;
  } else {
    Serial.print("led_status: ");
    Serial.println(response_UNAVAILABLE);
    responseCode = response_UNAVAILABLE;
  }

  doc["status"]["led"] = ledStatus;
  serializeJson(doc, buf);

  server.send(responseCode, F("application/json"), buf);
}

/**
 * Water level serial status request to sensor micro controller
 */
void requestWaterLevel() {
  Serial.println("GET /water");
}

/**
 * Water level response
 * wl: x, ps: x, id: xx:xx:xx
 *
 * Go to http://192.168.4.1/water
 */
void responseWaterLevel(String sensorMessage) {
  String waterLevelStatus;
  String pumpStatus;
  String toggleSinceDuration;
  String buf;
  DynamicJsonDocument doc(128);
  String responseBody;
  uint8_t responseCode;

  // wl: x
  // water_level: x
  waterLevelStatus = sensorMessage.substring(4, 5);
  waterLevelStatus.trim();
  waterLevelStatus = getWaterLevelMessage(waterLevelStatus);

  pumpStatus = sensorMessage.substring(11, 12);
  pumpStatus.trim();
  pumpStatus = pumpStatus == "1" ? "on" : "off";

  // xxx:xx:xx:xx
  toggleSinceDuration = sensorMessage.substring(18, 30);
  toggleSinceDuration.trim();
  toggleSinceDuration = toggleSinceDuration.toInt() >= 0 ? toggleSinceDuration : "ERROR";

  // Compose response
  Serial.println("water_level: ");
  if (waterLevelStatus == "empty" || waterLevelStatus == "low" || waterLevelStatus == "mid" || waterLevelStatus == "full") {
    Serial.println(response_OK);
    responseCode = response_OK;

    doc["water"]["level"] = waterLevelStatus;
    doc["water"]["irrigation"]["status"] = pumpStatus;
    doc["water"]["irrigation"]["since"] = toggleSinceDuration;

  } else {
    Serial.println(response_UNAVAILABLE);
    responseCode = response_UNAVAILABLE;

    doc["water"]["status"] = "ERROR";
  }

  serializeJson(doc, buf);
  server.send(responseCode, F("application/json"), buf);
}

/**
 * Convert sensor reading to user readable string for water level
 */
String getWaterLevelMessage(String waterLevelStatus) {
  // Check for error status
  if (waterLevelStatus == "-") {
    return "ERROR";
  }

  int waterLevel = waterLevelStatus.toInt();

  switch (waterLevel) {
    case 0:
      waterLevelStatus = "empty";
      break;

    case 1:
      waterLevelStatus = "low";
      break;

    case 2:
      waterLevelStatus = "mid";
      break;

    case 3:
      waterLevelStatus = "full";
      break;

    default:
      waterLevelStatus = "ERROR";
  }

  return waterLevelStatus;
}

/**
 * Water pump toggle serial request to sensor micro controller
 */
void requestIrrigateToggle() {
  Serial.println("GET /water/irrigate");
}

/**
 * Water irrigation status response
 *
 * sensorMessage: wl: x, is: x
 * serial log: water_level_status: xxx, irrigation_status: xxx
 *
 * Go to http://192.168.4.1/water/irrigate
 */
void responseIrrigate(String sensorMessage) {
  String waterLevelStatus;
  String irrigationStatus;
  String buf;
  DynamicJsonDocument doc(128);
  String responseBody;
  uint8_t responseCode;

  waterLevelStatus = sensorMessage.substring(4, 5);
  waterLevelStatus.trim();
  waterLevelStatus = getWaterLevelMessage(waterLevelStatus);
      
  irrigationStatus = sensorMessage.substring(11, 12);
  irrigationStatus.trim();
  irrigationStatus = irrigationStatus == "1" ? "on" : "off";

  // Compose response code
  Serial.print("irrigation_status: ");
  if (waterLevelStatus != "ERROR") {
    Serial.println(response_OK);
    responseCode = response_OK;

    // Compose response
    doc["water"]["level"] = waterLevelStatus;
    doc["water"]["irrigation"]["status"] = irrigationStatus;

  } else {
    Serial.println(response_UNAVAILABLE);
    responseCode = response_UNAVAILABLE;

    doc["water"]["status"] = "ERROR";
  }

  serializeJson(doc, buf);
  server.send(responseCode, F("application/json"), buf);
}

/**
 * Send HTTP status 404 (Not Found) when there's no handler for the URI
 * in the request
 */
void handleNotFound() {
  String buf;
  DynamicJsonDocument doc(64);

  doc["url"]["ok"] = false;

  serializeJson(doc, buf);
  server.send(response_NOT_FOUND , "application/json", buf);
}
