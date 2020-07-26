/**
 * HydroBites - ESP WaterStation webserver
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
  delay(1000);
  Serial.begin(115200); // Test at 9600 as UNO might need slower serial rate

  Serial.println();
  Serial.print("Configuring access point...");
  
  /* Password parameter removed to make open network.
   * WiFi.softAP(ssid, password);
   */
  WiFi.softAP(ssid);

  IPAddress stationIP = WiFi.softAPIP();
  Serial.println("AP (station) IP address: " + stationIP);

  Serial.println((String)"Local IP address: " + WiFi.localIP());

  // define routes
  server.on("/", handleRoot);
  // server.on("/", [](){server.send(200, "text/plain", "Hello World");
  
  server.begin();
  Serial.println("HTTP server started");
}

/**
 * Event loop
 */
void loop() {
  server.handleClient();
}

/*
 * Controllers
 * 
 * handleRoot
 * Go to http://192.168.4.1 in web browser for http access to network
 */
void handleRoot() {
  Serial.write("Request to /");
  server.send(200, "text/html", "<h1>You are connected to HydroBytes Water Station</h1>");
}
