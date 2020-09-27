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

  Serial.println("");
  Serial.println("HydroBites Water Station - Server");
  Serial.println("v0.1.1");
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

    if (sensorMessage.indexOf("serial_status:") >= 0) {
      responseStatus(sensorMessage);
    } else if (sensorMessage.indexOf("led_status:") >= 0) {
      responseLedStatus(sensorMessage);
    } else if (sensorMessage.indexOf("water_level:") >= 0) {
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
  // Write to serial channel to inform Ardunio (Uno) of event
  Serial.println("GET /");

  // JSON response of application status
  String body = "{\n";
  body += "  \"hydrobytes-water-station\": \"ok\"\n";
  body += "  \"release\": {\n";
  body += "    \"version\": \"v0.1.0\",\n";
  body += "    \"date\": \"30 August 2020\",\n";
  body += "  }\n";
  body += "  \"paths\": {\n";
  body += "    \"/\": \"Welcome\",\n";
  body += "    \"/status\": \"Current state of subsystems\"\n";
  body += "    \"/led\": \"Current state of LED - On/Off\"\n";
  body += "    \"/led/toggle\": \"Toggle LED On/Off\"\n";
  body += "    \"/water\": \"Current water level\"\n";
  body += "  }\n";
  body += "}";

  server.send(response_OK, "application/json", body);
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
  int responseCode = response_OK;

  // serial_status: x
  serialStatus = sensorMessage.substring(15, 19);
  // @todo: adjust to support sensor status value in addition to serial status
  sensorStatus = sensorMessage.substring(15, 19);

  String statusBody = "{\n";
  statusBody += "  \"two-way-serial-communication\": " + serialStatus + "\n";
  statusBody += "    \"water-sensors\": " + sensorStatus + "\n";
  statusBody += "}";

  if (serialStatus != "true" || sensorStatus != "true") {
    responseCode = response_UNAVAILABLE;
  }

  server.send(responseCode, "application/json", "{\n  status: " + statusBody + "\n}");
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
  String serialStatus;
  String responseBody;
  int responseCode;

  // serial_status: x
  serialStatus = sensorMessage.substring(12, 17);
  serialStatus.trim();

  responseBody = "{\n  status: {\n    led-on: " + serialStatus + "\n    }\n}";

  // Send response
  if (serialStatus == "true" || serialStatus == "false") {
    Serial.print("led_status: ");
    Serial.println(response_OK);
    responseCode = response_OK;
  } else {
    Serial.print("led_status: ");
    Serial.println(response_UNAVAILABLE);
    responseCode = response_UNAVAILABLE;
  }
  server.send(responseCode, "application/json", responseBody);
}

/**
 * Water level status request controller
 */
void requestWaterLevel() {
  Serial.println("GET /water");
}

/**
 *
 */
void responseWaterLevel(String sensorMessage) {
  String serialStatus;
  String responseBody;
  int responseCode;

  // serial_status: x
  serialStatus = sensorMessage.substring(13, 19);
  serialStatus.trim();
  responseBody = "{\n  water: {\n    level: " + serialStatus + "\n    }\n}";

  // Send response
  Serial.println("water_level: ");
  if (serialStatus == "empty" || serialStatus == "low" || serialStatus == "midway" || serialStatus == "full") {
    Serial.println(response_OK);
    responseCode = response_OK;
  } else {
    Serial.println(response_UNAVAILABLE);
    responseCode = response_UNAVAILABLE;
  }
  server.send(responseCode, "application/json", responseBody);
}

/**
 * Send HTTP status 404 (Not Found) when there's no handler for the URI
 * in the request
 */
void handleNotFound() {
  server.send(response_NOT_FOUND , "application/json", "{\n  status: Not Found\n}");
}
