/**
   HydroBytes ESP-01 test case 03

   Control LED and recieve notification of button press via wifi
*/

// Include the SoftwareSerial library to allow pins 6 and 7 to receive
// serial communication from the ESP-01
#include <SoftwareSerial.h>

//  Assign the name “LED” to pin 5 and “button” to pin 11, and make the initial value of the button zero.
#define TIMEOUT 10000 // mS
#define LED 5

SoftwareSerial mySerial(7, 6); // RX, TX
const int button = 11;
int button_state = 0;

/**
   Run once setup logic
*/
void setup()
{
  // Make the LED an output and the button an input.
  pinMode(LED, OUTPUT);
  pinMode(button, INPUT);

  // Begin serial communication, define baud rate for both channels of communication
  Serial.begin(115200); // 115200 / 9600
  mySerial.begin(115200);

  // Configure the ESP-01
  // Reset the module and wait for finish
  Serial.println("setup() Reset the module and wait for finish: Sending AT+RST");
  SendCommand("AT+RST", "Ready");
  delay(15000);

  Serial.println("DONE delay(15000)");

//  // Enter "STA" (station) mode - connect to router
//  Serial.println("setup() Enter STA (station) mode - connect to router: Sending AT+CWMODE=1");
//  SendCommand("AT+CWMODE=1", "OK");
//
//  // request IP address
//  Serial.println("setup() request IP address: Sending AT+CIFSR");
//  SendCommand("AT+CIFSR", "OK");
//
//  // Enable multiple connections
//  Serial.println("setup() Enable multiple connections: Sending AT+CIPMUX=1");
//  SendCommand("AT+CIPMUX=1", "OK");
//
//  // start server on port 80
//  Serial.println("setup() start server on port 80: Sending AT+CIPSERVER=1,80");
//  SendCommand("AT+CIPSERVER=1,80", "OK");
}


/**
   Run in infinite loop - polling logic. Until application exits or goes to sleep
*/
void loop()
{
 // Serial.println("loop() start...");

//  // read the state of the button
//  button_state = digitalRead(button);
//  Serial.println("loop() button_state: ");
//
//  // Is the button pressed
//  if (button_state == HIGH)
//  {
//    //send 23 bits of data through channel 0 to the device connected to the ESP-01
//    mySerial.println("AT+CIPSEND=0,23");
//
//    // 23 bits
//    mySerial.println("<h1>Button was pressed!</h1>");
//    delay(1000);
//
//    // Close coonnection to complete sending
//    SendCommand("AT+CIPCLOSE=0", "OK");
//  }
//
//
//  // Variable to hold the data coming from the ESP module
//  String IncomingString = "";
//  boolean StringReady = false;
//
//  while (mySerial.available()) {
//    IncomingString = mySerial.readString();
//    StringReady = true;
//  }
//
//  // String command received
//  if (StringReady) {
//    Serial.println("Received String: " + IncomingString);
//
//    // Turn LED On
//    if (IncomingString.indexOf("LED=ON") != -1) {
//      digitalWrite(LED, HIGH);
//    }
//
//    // Turn Led Off
//    if (IncomingString.indexOf("LED=OFF") != -1) {
//      digitalWrite(LED, LOW);
//    }
//  }
}

/**
   SendCommand - Format and send AT seriel command to ESP-01

   cmd String Command to send to ESP-01
   ack String Expected acknowledgment from ESP-01
*/
boolean SendCommand(String cmd, String ack)
{
  // Send "AT+" command to module
  mySerial.println(cmd);

  if (!echoFind(ack)) // timed out waiting for ack string
    return true; // ack blank or ack found
}

/**
   echoFind - Compare the acknowledge string to the response given by the ESP-01 once a
   command has been sent
*/
boolean echoFind(String keyword)
{
  Serial.println("echoFind() keyword: " + keyword);
  
  // index to compare each character between the acknowledge string and the ESP response
  byte current_char = 0;

  // the length of the keyword, used to limit the compairison scan
  byte keyword_length = keyword.length();

  // the time since the Arduino board began running plus TIMEOUT (5000ms)
  long deadline = millis() + TIMEOUT;

  while (millis() < deadline)
  {
    Serial.println((String)"echoFind() millis() < deadline: " + millis() + " < " + deadline);
    
    if (mySerial.available())
    {
      Serial.println("echoFind() mySerial.available()");
      
      char ch = mySerial.read();
      Serial.write(ch);
      Serial.println("ch: " + ch);

      if (ch == keyword[current_char])
        if (++current_char == keyword_length)
        {
          Serial.println();
          return true;
        }
    }
  }

  Serial.println("echoFind() return false");

  // Timed out
  return false;
}
