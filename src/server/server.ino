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
  Serial.println("v0.2.1");
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

    // irrigation_status
    } else if (sensorMessage.indexOf("is:") >= 0) {
      responseIrrigate(sensorMessage);

    // water_level
    } else if (sensorMessage.indexOf("wl:") >= 0) {
      responseWaterLevel(sensorMessage);
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

  doc["release"]["version"] = "v0.2.0";
  doc["release"]["date"] = "18 October 2020";

  doc["paths"]["/"] = "Welcome";
  doc["paths"]["/status"] = "Current state of subsystem";
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
 * LED status request controller
 */
void requestLedStatus() {
  Serial.println("GET /led");
}

/**
 * LED toggle request controller
 */
void requestLedToggle() {
  Serial.println("GET /led/toggle");
}

/**
 * Led status response
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
 * Water level status request controller
 */
void requestWaterLevel() {
  Serial.println("GET /water");
}

/**
 * Water level response
 */
void responseWaterLevel(String sensorMessage) {
  String waterLevelStatus;
  String buf;
  DynamicJsonDocument doc(64);
  String responseBody;
  uint8_t responseCode;

  // wl: x
  // water_level: x
  waterLevelStatus = sensorMessage.substring(4, 5);
  waterLevelStatus.trim();
  waterLevelStatus = getWaterLevelMessage(waterLevelStatus);

  // Compose response
  Serial.println("water_level: ");
  if (waterLevelStatus == "empty" || waterLevelStatus == "low" || waterLevelStatus == "mid" || waterLevelStatus == "full") {
    Serial.println(response_OK);
    responseCode = response_OK;
  } else {
    Serial.println(response_UNAVAILABLE);
    responseCode = response_UNAVAILABLE;
  }

  doc["water"]["level"] = waterLevelStatus;

  serializeJson(doc, buf);
  server.send(responseCode, F("application/json"), buf);
}

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
 * Water pump toggle request controller
 */
void requestIrrigateToggle() {
  Serial.println("GET /water/irrigate");
}

/**
 * Water irrigation status response
 */
void responseIrrigate(String sensorMessage) {
  String waterLevelStatus;
  String irrigationStatus;
  String irrigationDuration;
  String buf;
  DynamicJsonDocument doc(128);
  String responseBody;
  uint8_t responseCode;

  // water_level_status: x, irrigation_status: x, irrigation_duration: x
  // wl: x, is: x, id: x
  waterLevelStatus = sensorMessage.substring(4, 5);
  waterLevelStatus.trim();

  waterLevelStatus = getWaterLevelMessage(waterLevelStatus);
      
  irrigationStatus = sensorMessage.substring(11, 12);
  irrigationStatus.trim();
  irrigationStatus = irrigationStatus == "1" ? "on" : "off";

  irrigationDuration = sensorMessage.substring(18);
  irrigationDuration.trim();
  irrigationDuration = irrigationDuration.toInt() >= 0 ? irrigationDuration : "ERROR";

  // Compose response code
  Serial.print("irrigation_status: ");
  if (waterLevelStatus != "ERROR" && irrigationDuration != "ERROR") {
    Serial.println(response_OK);
    responseCode = response_OK;
  } else {
    Serial.println(response_UNAVAILABLE);
    responseCode = response_UNAVAILABLE;
  }

  // Compose response
  doc["water"]["level"] = waterLevelStatus;
  doc["water"]["irrigation"]["status"] = irrigationStatus;
  doc["water"]["irrigation"]["duration"] = irrigationDuration;

  serializeJson(doc, buf);
  server.send(responseCode, F("application/json"), buf);
}

/**
 * Send HTTP status 404 (Not Found) when there's no handler for the URI
 * in the request
 */
void handleNotFound() {
  server.send(response_NOT_FOUND , "application/json", "{\n  status: Not Found\n}");
}
