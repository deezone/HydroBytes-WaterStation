#include <SoftwareSerial.h>

SoftwareSerial wifiSerial(2, 3);      // RX, TX for ESP8266

// show more logs
bool DEBUG = true;

// communication timeout
int responseTime = 10;

void setup()
{
  //set build in led as output
  pinMode(13, OUTPUT);
  
  // Open serial communications and wait for port to open esp8266:
  Serial.begin(115200);

  wifiSerial.begin(115200);

  // configure as access point
  sendToWifi("AT+CWMODE=2", responseTime, DEBUG);

  // get ip address
  sendToWifi("AT+CIFSR", responseTime, DEBUG);

  // configure for multiple connections
  sendToWifi("AT+CIPMUX=1", responseTime, DEBUG);

  // turn on server on port 80
  sendToWifi("AT+CIPSERVER=1,80", responseTime, DEBUG);
 
  sendToUno("Wifi connection is running!", responseTime, DEBUG);
}

void loop() {
  // put your main code here, to run repeatedly:

}

/*
 * Name: sendToWifi
 * Description: Function used to send data to ESP8266.
 * Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
 * Returns: The response from the esp8266 (if there is a reponse)
 */
String sendToWifi(String command, const int timeout, boolean debug){
  String response = "";
  long int time = millis();

  // send the read character to the esp8266
  wifiSerial.println(command);

  while( (time+timeout) > millis())
  {
    while(wifiSerial.available())
    {
    // The esp has data so display its output to the serial window 
    char c = wifiSerial.read(); // read the next character.
    response+=c;
    }  
  }
  
  if(debug)
  {
    Serial.println(response);
  }
  
  return response;
}

/*
 * Name: sendToUno
 * Description: Function used to send data to Arduino.
 * Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
 * Returns: The response from the esp8266 (if there is a reponse)
 */
String sendToUno(String command, const int timeout, boolean debug) {
  String response = "";
  long int time = millis();

  // send the read character to the esp8266
  Serial.println(command);
  

  while((time+timeout) > millis())
  {
    while(Serial.available())
    {
      // The esp has data so display its output to the serial window 
      char c = Serial.read(); // read the next character.
      response+=c;
    }  
  }

  if(debug)
  {
    Serial.println(response);
  }

  return response;
}
