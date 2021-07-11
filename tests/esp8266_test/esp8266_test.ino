/**
 * HydroBytes Water Station
 * 
 * ESP8266WiFi
 * https://arduino-esp8266.readthedocs.io/en/stable/esp8266wifi/readme.html#
 *
 * ESP8266WebServer
 * https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
 * 
 * 
 * ArduinoJson
 * 
 */
#include <string.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const uint16_t PORT = 7000;
const uint16_t BAUD_RATE = 9600;
const byte MAX_STATUS_MESSAGE_LEN = 32;

const char *SERVER_VERSION = "v0.4.0";
const char *SERVER_RESEASE = "15 May 2021";

// uintxx_t: 8 bit: 0-255
const uint8_t RESPONSE_OK_CODE = 200;
const uint16_t RESPONSE_NOT_FOUND_CODE = 404;
const uint16_t RESPONSE_UNAVAILABLE_CODE = 503;

const char *RESPONSE_NOT_FOUND_MESSAGE = "URL not found";

// serial communication globals
char receivedChars[MAX_STATUS_MESSAGE_LEN];
boolean newData = false;

// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server(PORT);

void setup() {

  // Wait for serial port to be available
  // Include a delay in the routine. That would allow background tasks that need to run. You can not have long blocking
  // code with the ESP8266. It will cause the behavior:

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
  // delay(10000);

  Serial.println("HydroBites Water Station - Server");
  Serial.println(SERVER_VERSION);
  Serial.println(SERVER_RESEASE);
  Serial.println("");
  
  Serial.begin(BAUD_RATE);
  Serial.println();

  WiFi.begin("KyKyRyKyKyy", "pond482dan446twin");

  Serial.print("Connecting at: ");
  Serial.print(BAUD_RATE);
  Serial.print(" baud");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(PORT);

  // server routers
  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, requestStatus);
//  server.on("/led", HTTP_GET, requestLedStatus);
//  server.on("/led/toggle", HTTP_GET, requestLedToggle);
//  server.on("/water", HTTP_GET, requestWaterLevel);
//  server.on("/water/irrigate", HTTP_GET, requestIrrigateToggle);
  server.onNotFound(handleNotFound);

  //Start the server
  server.begin();

  Serial.println("Station listening ~~~");
  Serial.flush();
}

void loop() {
  char* serialStatus;

  // Serial messages from sensors (Uno)
  recvWithEndMarker();   

  if (newData == true) {

    // GET /status
    // serial_status (sr), sensor_status (ss), clock_status (cs)
    // sr:x,ss:x,cs:x;
    // sr:1,ss:1,cs:1
    serialStatus = subStr(receivedChars, ",", 1);
//    char serialValue[MAX_STATUS_MESSAGE_LEN] = { " " };
//    serialValue = *serialStatus;
    
    Serial.print("loop() serialStatus: ");
////    Serial.println(subStr(receivedChars, ",", 1));
    Serial.println(serialStatus);
//
//     Serial.print("serialValue: ");
//    Serial.println(serialValue);

    if (*(serialStatus) == 's') {
      Serial.println("serialStatus 's' - sr:1 TRUE");
    }

    if (serialStatus == "sr:1") {
      Serial.println("serialValue 'sr:1' TRUE");
    }

//
//    char *serialStatusValue = strchr(serialStatus, ':');
//    Serial.print("serialStatusValue: ");
//    Serial.println(serialStatusValue);
//
//    Serial.print("serialStatus == "sr:1" ");
//    String temp = serialStatusValue == ":1" ? "true" : "false";
//    Serial.println(temp);

    
    
//    if (serialStatus == "sr:1" || serialStatus == "sr:0") {
//      responseStatus();
//    }

    newData = false;
  }
  




//    // led_status
//    ls:x
//    } else if (sensorMessage.indexOf("ls:") >= 0) {
//      responseLedStatus(sensorMessage);
//
//    // water_status (wl:x,ps:x,id:xx:xx:xx)
//    } else if (sensorMessage.indexOf("id:") >= 0) {
//      responseWaterLevel(sensorMessage);
//
//    // irrigation_status (wl: x, ps: x)
//    } else if (sensorMessage.indexOf("wl:") >= 0) {
//      responseIrrigate(sensorMessage);


//  // Handling of incoming client requests
//  server.handleClient();
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

  // Log request to serial channel
  Serial.println("GET /");

  // Compose response
  doc["hydrobytes-water-station"] = "ok";
  doc["status"]["code"] = RESPONSE_OK_CODE;

  doc["release"]["version"] = SERVER_VERSION;
  doc["release"]["date"] = SERVER_RESEASE;

  doc["paths"]["/"] = "Welcome";
  doc["paths"]["/status"] = "Current health of subsystems";
  doc["paths"]["/led"] = "Current state of LED - on/off";
  doc["paths"]["/led/toggle"] = "Toggle LED on/off";
  doc["paths"]["/water"] = "Current water status";
  doc["paths"]["/water/irrigate"] = "Toggle irrigation on/off";

  serializeJson(doc, buf);

  server.send(RESPONSE_OK_CODE, F("application/json"), buf);
}

/**
 * Status request controller
 */
void requestStatus() {
  Serial.println("GET /status");
}

/**
 * Status request response. Returns http response to server http request to GET /status based on
 * status reponse from sensor controller.
 *
 * Go to 192.168.1.197:7000/status
 */
void responseStatus() {
  char* statusMessage;
  char serialStatus[5];
//  char* sensorStatus;
//  char* clockStatus;

  String buf;
  DynamicJsonDocument doc(256);
  uint16_t responseCode = RESPONSE_OK_CODE;

  // serial_status (sr), sensor_status (ss), clock_status (cs)
  // sr:x,ss:x,cs:x;
  // Serial status
  statusMessage = subStr(receivedChars, ",", 1);

  Serial.print("statusMessage: ");
  Serial.println(statusMessage);
  
  // serialStatus = statusMessage == "sr:1" ? "true" : "false";

  strncpy(serialStatus, statusMessage == "sr:1" ? "true" : "false", sizeof(serialStatus)-1);

  // sr:1,ss:1,cs:1
  Serial.print("serialStatus: ");
  Serial.println(serialStatus);

//  // Sensor status
//  statusMessage = subStr(sensorMessage, ",", 2);
//  sensorStatus = = statusMessage == "ss:1" ? "true" : "false";
//
//  // Clock status
//  statusMessage = subStr(sensorMessage, ",", 3);
//  clockStatus = statusMessage == "cs:1" ? "true" : "false";

  // Unhealthy status
//  if (serialStatus != "true" || sensorStatus != "true" || clockStatus != "true") {
//    responseCode = RESPONSE_UNAVAILABLE_CODE;
//  }

 // doc["status"]["two-way-serial-communication"] = serialStatus;
//  doc["status"]["water-sensors"] = sensorStatus;
//  doc["status"]["real-time-clock"] = clockStatus;
//  serializeJson(doc, buf);
//
//  server.send(responseCode, F("application/json"), buf);
}

/**
 * Send HTTP status 404 (Not Found) when there's no handler for the URI
 * in the request
 */
void handleNotFound() {
  String buf;
  DynamicJsonDocument doc(128);

  doc["status"]["code"] = RESPONSE_NOT_FOUND_CODE;
  doc["status"]["message"] = RESPONSE_NOT_FOUND_MESSAGE;

  serializeJson(doc, buf);
  server.send(RESPONSE_NOT_FOUND_CODE , "application/json", buf);
}
/**
 *
 */
void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
    
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
  
    if (rc != endMarker) {

      receivedChars[ndx] = rc;
      ndx++;

      if (ndx >= MAX_STATUS_MESSAGE_LEN) {
        ndx = MAX_STATUS_MESSAGE_LEN - 1;
      }

    } else {

      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;

    }
  }
}

// Function to return a substring defined by a delimiter at an index
// https://forum.arduino.cc/t/help-with-strtok-function/41499/9
char* subStr (char* str, char *delim, int index) {
   char *act, *sub, *ptr;
   static char copy[MAX_STATUS_MESSAGE_LEN];
   int i;

   // Since strtok consumes the first arg, make a copy
   strcpy(copy, str);

   for (i = 1, act = copy; i <= index; i++, act = NULL) {
      sub = strtok_r(act, delim, &ptr);
      if (sub == NULL) break;
   }
   
   return sub;
}
