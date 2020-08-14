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

  // while(!Serial);
  delay(1000);

  Serial.begin(9600); // Test at 115200/9600 as UNO/ESP might need slower serial rate

  Serial.println("Configuring access point...");
  
  /* Password parameter removed to make open network.
   * WiFi.softAP(ssid, password);
   */
  Serial.print("Setting soft-AP ... ");
  WiFi.softAP(ssid) ? "Ready" : "Failed!";

  IPAddress stationIP = WiFi.softAPIP();
  Serial.println("AP (station) IP address: " + stationIP);

  Serial.println("Local IP address: " + WiFi.localIP());
  Serial.flush();

  // define routes
  server.on("/",    handleRoot);
  
  server.begin();
  Serial.println("HTTP server started");
}

/**
 * Event loop
 */
void loop() {
  String incomingString="";

  while(Serial.available()) {
    incomingString = Serial.readStringUntil('\n');
    Serial.println((String)"server received: " + incomingString);

    Serial.println("server -> PING");
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
  Serial.write("/");

  // JSON response of application status
  server.send(200, "application/json", "{\n  status: ok\n}");
}

/**
 * Send HTTP status 404 (Not Found) when there's no handler for the URI
 * in the request
 */
void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}
