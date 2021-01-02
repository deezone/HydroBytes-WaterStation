#include <Arduino.h>
int ledpin=13;// initialize pin 13
int inpin=7;// initialize pin 7
int val;// define val

void setup() {
    Serial.begin(9600);
    pinMode(ledpin,OUTPUT);// set LED pin as “output”
    pinMode(inpin,INPUT);// set button pin as “input”
}

void loop() {
    val=digitalRead(inpin);// read the level value of pin 7 and assign if to val
    Serial.println(val); // print the data from the sensor
    delay(100);
    if(val==LOW)// check if the button is pressed, if yes, turn on the LED
    { digitalWrite(ledpin,LOW);}
    else
    { digitalWrite(ledpin,HIGH);}
}
