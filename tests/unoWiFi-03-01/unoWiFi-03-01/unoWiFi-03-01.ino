#include <SoftwareSerial.h>

SoftwareSerial wifiSerial(6, 7);      // RX, TX for ESP8266

bool DEBUG = true;   //show more logs
int responseTime = 100; //communication timeout

void setup()
{
  pinMode(13,OUTPUT);  //set build in led as output

  // Open serial communications and wait for port to open esp8266:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("serial communications and port to open esp8266 open");
  
  wifiSerial.begin(115200);
  while (!wifiSerial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("serial port conneted");

  sendToWifi("AT+RST", responseTime, DEBUG);
  delay(5000);
  
//  sendToWifi("AT+CWMODE=2", responseTime, DEBUG); // configure as access point
//  sendToWifi("AT+CIFSR", responseTime, DEBUG); // get ip address
//  sendToWifi("AT+CIPMUX=1", responseTime, DEBUG); // configure for multiple connections
//  sendToWifi("AT+CIPSERVER=1,80", responseTime, DEBUG); // turn on server on port 80
 
  // sendToUno("Wifi connection is running!",responseTime,DEBUG);
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
String sendToWifi(String command, const int timeout, boolean debug)
{
  String response = "";
  Serial.println("command: " + command);
  wifiSerial.println(command); // send the read character to the esp8266
  
  long int time = millis();
  while((time+timeout) > millis())
  {
    Serial.println((String)"wifiSerial.available: " + wifiSerial.available());
    
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
