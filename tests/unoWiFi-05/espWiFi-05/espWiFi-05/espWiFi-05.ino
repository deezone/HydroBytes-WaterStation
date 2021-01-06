/**
 * HydroBites - ESP WaterStation webserver
 * v0.0.1
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

ESP8266WebServer server(80);

/**
 * Setup
 */
void setup() {
  delay(1000);
  Serial.begin(115200);

  Serial.println();
  Serial.print("Configuring access point...");
  
  /* Password parameter removed to make open network.
   * WiFi.softAP(ssid, password);
   */
  WiFi.softAP(ssid);

  IPAddress stationIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(stationIP);

  // define routes
  server.on("/", handleRoot);
  
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
