/*
* Arduino DS3231 Real Time Clock Module Tutorial
*
* Crated by Dejan Nedelkovski,
* www.HowToMechatronics.com
*
* DS3231 Library made by Henning Karlsen which can be found and downloaded
* from his website, www.rinkydinkelectronics.com.
* 
* Arduino Uno/2009:
* ----------------------
* DS3231:  SDA pin   -> Arduino Analog 4 or the dedicated SDA pin
*          SCL pin   -> Arduino Analog 5 or the dedicated SCL pin
*
* You can connect the DS3231 to any available pin but if you use any
* other than what is described above the library will fall back to
* a software-based, TWI-like protocol which will require exclusive access 
* to the pins used, and you will also have to use appropriate, external
* pull-up resistors on the data and clock signals.
*
*/
#include <SoftwareSerial.h>
#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// Init a Time-data structure
Time  t;

// Time to track time between when water pump is tunnered on/off
unsigned long previousIrrigationTime = rtc.getUnixTime(rtc.getTime());

const String server_VERSION = "v0.3.0";
const String server_RELEASE = "15 May 2021";

const bool DEBUG = false;

// initialize pins
const uint8_t waterSensorLowLedPin  = 2;
const uint8_t waterSensorMidLedPin  = 3;
const uint8_t waterSensorHighLedPin = 4;

const uint8_t waterPumpRelayPin = 5;

const int waterSensorLowPin  = 6;
const int waterSensorMidPin  = 7;
const int waterSensorHighPin = 8;

// used by ESP8266 for serial communication
SoftwareSerial serverSerial(10, 11); // Rx, Tx

const uint8_t ledPin = 12;

const uint8_t waterLevel_empty = 0;
const uint8_t waterLevel_low   = 1;
const uint8_t waterLevel_mid   = 2;
const uint8_t waterLevel_high  = 3;

void setup() {
  // Setup Serial connection
  Serial.begin(9600);
  Serial.println("Sensors: serial connection started...");
  Serial.println("");

  Serial.println("HydroBites Water Station");
  Serial.println(server_VERSION);
  Serial.println(server_RELEASE);
  Serial.println("");

  Serial.println("Sensors: Sensors <-> Server serial connection started...");
  serverSerial.begin(9600);
  
  // Initialize the rtc object
  rtc.begin();
  
  // The following lines can be uncommented to set the date and time
  //  rtc.setDOW(SATURDAY);     // Set Day-of-Week to SUNDAY
  //  rtc.setTime(19, 52, 0);     // Set the time to 12:00:00 (24hr format)
  //  rtc.setDate(15, 5, 2021);   // Set the date to 1st January 2014
}

void loop() {

  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.println(rtc.getTimeStr());

  Serial.println("");
  Serial.println("~~~~~~~~~~~~~~");
  Serial.println("");

  // Send Unixtime
  // ** Note that there may be a one second difference between the current time **
  // ** and current unixtime show if the second changes between the two calls   **
  Serial.print("Current Unixtime.........................: ");
  Serial.println(rtc.getUnixTime(rtc.getTime()));
  
  // Send Unixtime for 00:00:00 on January 1th 2014
  Serial.print("Unixtime for 00:00:00 on January 1th 2014: ");
  t.hour = 0;
  t.min = 0;
  t.sec = 0;
  t.year = 2014;
  t.mon = 1;
  t.date = 1;
  Serial.println(rtc.getUnixTime(t));

  Serial.println("");
  Serial.println("~~~~~~~~~~~~~~");
  Serial.println("");

  // Get data from the DS3231
  t = rtc.getTime();
  
  // Send date over serial connection
  Serial.print("Today is the ");
  Serial.print(t.date, DEC);
  Serial.print(". day of ");
  Serial.print(rtc.getMonthStr());
  Serial.print(" in the year ");
  Serial.print(t.year, DEC);
  Serial.println(".");
  
  // Send Day-of-Week and time
  Serial.print("It is the ");
  Serial.print(t.dow, DEC);
  Serial.print(". day of the week (counting monday as the 1th), and it has passed ");
  Serial.print(t.hour, DEC);
  Serial.print(" hour(s), ");
  Serial.print(t.min, DEC);
  Serial.print(" minute(s) and ");
  Serial.print(t.sec, DEC);
  Serial.println(" second(s) since midnight.");

  // Send a divider for readability
  Serial.println("  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -");

  // Send current temperature
  Serial.print("Temperature: ");
  Serial.print(rtc.getTemp());
  Serial.println(" C");
  
  // Wait one second before repeating :)
  delay (1000);
}
