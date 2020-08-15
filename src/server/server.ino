/**
 * HydroBites WaterStation http server
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

/* WiFi credentials. */
#ifndef APSSID
#define APSSID "HydroBytes_WaterStation"
#define APPSK  "undefined" // password
#endif

const char *ssid = APSSID;
const char *password = APPSK;

// Instantiate 
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
  
  server.begin();
  Serial.println("HTTP server started");
  Serial.flush();
}

/**
 * Event loop
 */
void loop() {
  String sensorMessage;

  // Serial messages from sensor (Uno)
  while (Serial.available()) {
    sensorMessage = Serial.readStringUntil('\n');

    if (sensorMessage.indexOf("serial_status:") >= 0) {
      responseStatus(sensorMessage);
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
  body += "  \"hydrobytes-waters-station\": \"ok\"\n";
  body += "  \"paths\": {\n";
  body += "    \"/\": \"Welcome message\",\n";
  body += "    \"/status\": \"Current state of subsystems\"\n";
  body += "  }\n";
  body += "}";

  server.send(200, "application/json", body);
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

  // serial_status: x
  serialStatus = sensorMessage.substring(15, 19);

  // Send response
  if (serialStatus == "true") {
    Serial.println("200");
    server.send(200, "application/json", "{\n  status: {\n    two-way-serial-communicstion: " + serialStatus + "\n    }\n}");
  } else {
    Serial.println("503");
    server.send(503, "application/json", "{\n  status: {\n    two-way-serial-communicstion: false\n    }\n}");
  }
}

/**
 * Send HTTP status 404 (Not Found) when there's no handler for the URI
 * in the request
 */
void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}