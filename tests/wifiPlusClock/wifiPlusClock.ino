/**
 * HydroBites WaterStation sensors
 * 
 * DS3231
 * https://github.com/Makuna/Rtc
 * 
 */
#include <SoftwareSerial.h>

/* 
 * for software wire use below

#include <SoftwareWire.h>  // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
SoftwareWire myWire(SDA, SCL);
RtcDS3231<SoftwareWire> Rtc(myWire);

 * for software wire use above 
 */

/* for normal hardware wire use */
//#include <Wire.h> // must be included here so that Arduino library object file references work
//#include <RtcDS3231.h>
//RtcDS3231<TwoWire> Rtc(Wire);

// Current time, used to track interval between when water pump is turned on/off
// unsigned long previousIrrigationTime = Rtc.GetDateTime();

const String server_VERSION = "v0.3.0";
const String server_RELEASE = "15 May 2021";

// used by ESP8266 for serial communication
SoftwareSerial serverSerial(10, 11); // Rx, Tx

void setup() {
  Serial.begin(9600);
  Serial.println("Sensors: serial connection started...");
  Serial.println("");

  Serial.println("HydroBites Water Station");
  Serial.println(server_VERSION);
  Serial.println(server_RELEASE);
  Serial.println("");

  Serial.println("Sensors: Sensors <-> Server serial connection started...");
  serverSerial.begin(9600);

  Serial.println("Sensors: setup complete...");

    // Initialize the rtc (real-time clock) object
//  Rtc.Begin();
//
//  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
//  printDateTime(compiled);
//  Serial.println();
//
//  if (!Rtc.IsDateTimeValid()) {
//    Serial.print("WARNING: RTC Invalid Date");
//          
//    if (Rtc.LastError() != 0) {
//
//      // communications error
//      // see https://www.arduino.cc/en/Reference/WireEndTransmission for explination of number values
//      Serial.print("RTC communications error = ");
//      Serial.println(Rtc.LastError());
//
//    } else {
//      
//      // Common Causes:
//      //   1) first time you ran and the device wasn't running yet
//      //   2) the battery on the device is low or even missing
//      Serial.println("RTC - lost confidence in the DateTime!");
//
//      // set the RTC to the date and time this sketch was compiled
//      // - also resets valid flag internally unless the Rtc device is having an issue
//      Rtc.SetDateTime(compiled);
//      Serial.println("RTC set to compile time");
//    }
//  }
//
//  if (!Rtc.GetIsRunning()) {
//    Serial.println("RTC was not actively running, starting now");
//    Rtc.SetIsRunning(true);
//  }
//
//
//  // RTC setting check
//  RtcDateTime now = Rtc.GetDateTime();
//  if (now < compiled) {
//    Serial.println("RTC is older than compile time!  (Updating DateTime)");
//    Rtc.SetDateTime(compiled);
//  } else if (now > compiled) {
//    Serial.println("RTC is newer than compile time. (this is expected)");
//  } else if (now == compiled) {
//    Serial.println("RTC is the same as compile time! (not expected but all is good... what are the chances!?!)");
//  }
//
//  // Do not assume the Rtc was last configured correctly
//  // Clear RTC setting to needed state
//  Rtc.Enable32kHzPin(false);
//  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
//
//  Serial.println("Real-Time Clock: Initialized...");
//  Serial.println();

  /**
     Serial.write are transmitted in the background (from an interrupt handler) allowing your
     sketch code to immediately resume processing. This is usually a good thing (it can make the
     sketch more responsive) but sometimes you want to wait until all characters are sent. You can
     achieve this by calling Serial.flush() immediately following Serial.write().
  */
  Serial.flush();

}

void loop() {
  String serverMessage;
  int irrigationDuration = 0;

//  if (!Rtc.IsDateTimeValid()) {
//    
//    if (Rtc.LastError() != 0) {
//      
//      // we have a communications error
//      // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
//      // what the number means
//      Serial.print("RTC communications error = ");
//      Serial.println(Rtc.LastError());
//      
//    } else {
//      // Common Causes
//      //  1) the battery on the device is low or even missing and the power line was disconnected
//      Serial.println("RTC lost confidence in the DateTime!");
//    }
//
//  }
//
//  RtcDateTime now = Rtc.GetDateTime();
//  Serial.print("RTC time: ");
//  printDateTime(now);
//  Serial.println();
//
//  RtcTemperature temp = Rtc.GetTemperature();
//  Serial.print("RTC temperature: ");
//
//  // you may also get the temperature as a float and print it
//  // Serial.print(temp.AsFloatDegC());
//  temp.Print(Serial);
//  Serial.println("C");

Serial.print("serverSerial.available(): ");
if (serverSerial.available()) {
  Serial.println("true");
} else {
    Serial.println("false");
}


  while (serverSerial.available()) {
    serverMessage = serverSerial.readStringUntil('\n');
    Serial.println((String)"Server: " + serverMessage);


  }

}

//#define countof(a) (sizeof(a) / sizeof(a[0]))
//
//void printDateTime(const RtcDateTime& dt) {
//  char datestring[20];
//
//  snprintf_P(
//    datestring, 
//    countof(datestring),
//    PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
//    dt.Month(),
//    dt.Day(),
//    dt.Year(),
//    dt.Hour(),
//    dt.Minute(),
//    dt.Second()
//  );
//  
//  Serial.print(datestring);
//}
