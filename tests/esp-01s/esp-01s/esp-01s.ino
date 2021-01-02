// https://www.reddit.com/r/esp8266/comments/ayioc2/esp01_vs_esp01s_affecting_my_code/
// Here's the code. The esp-01 simply hosts a webpage with 2 buttons to turn 2 LEDS on or off through the 2 available GPIO pins, through
// the miracle of wireless...

// Load Wi-Fi library

#include <ESP8266WiFi.h>



// Replace with your network credentials

const char* ssid = "RT-AC66U_B1_88_2G";

const char* password = "history_6524";

// Set web server port number to 80

WiFiServer server(80);

// Variable to store the HTTP request

String header;



// Auxiliar variables to store the current output state

String output5State = "off";

String output4State = "off";



// Assign output variables to GPIO pins //GPIO 0 and 2 on esp-01

const int output5 = 2;

const int output4 = 0;



void setup() {

Serial.begin(115200);

// Initialize the output variables as outputs

pinMode(output5, OUTPUT);

pinMode(output4, OUTPUT);

// Set outputs to LOW

digitalWrite(output5, LOW);

digitalWrite(output4, LOW);



// Connect to Wi-Fi network with SSID and password

Serial.print("Connecting to ");

Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {

delay(500);

Serial.print(".");

}

// Print local IP address and start web server

Serial.println("");

Serial.println("WiFi connected.");

Serial.println("IP address: ");

Serial.println(WiFi.localIP());

server.begin();

}



void loop(){

WiFiClient client = server.available(); // Listen for incoming clients



if (client) { // If a new client connects,

Serial.println("New Client."); // print a message out in the serial port

String currentLine = ""; // make a String to hold incoming data from the client

while (client.connected()) { // loop while the client's connected

if (client.available()) { // if there's bytes to read from the client,

char c = client.read(); // read a byte, then

Serial.write(c); // print it out the serial monitor

header += c;

if (c == '\n') { // if the byte is a newline character

// if the current line is blank, you got two newline characters in a row.

// that's the end of the client HTTP request, so send a response:

if (currentLine.length() == 0) {

// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)

// and a content-type so the client knows what's coming, then a blank line:

client.println("HTTP/1.1 200 OK");

client.println("Content-type:text/html");

client.println("Connection: close");

client.println();

// turns the GPIOs on and off

if (header.indexOf("GET /5/on") >= 0) {

Serial.println("GPIO 5 on");

output5State = "on";

digitalWrite(output5, HIGH);

} else if (header.indexOf("GET /5/off") >= 0) {

Serial.println("GPIO 5 off");

output5State = "off";

digitalWrite(output5, LOW);

} else if (header.indexOf("GET /4/on") >= 0) {

Serial.println("GPIO 4 on");

output4State = "on";

digitalWrite(output4, HIGH);

} else if (header.indexOf("GET /4/off") >= 0) {

Serial.println("GPIO 4 off");

output4State = "off";

digitalWrite(output4, LOW);

}

// Display the HTML web page

-******** I removed this code to make the code fit into this reddit post ********

// Display current state, and ON/OFF buttons for GPIO 5

-******** I removed this code to make the code fit into this reddit post ********

// Display current state, and ON/OFF buttons for GPIO 4

-******** I removed this code to make the code fit into this reddit post ********

// The HTTP response ends with another blank line

client.println();

// Break out of the while loop

break;

} else { // if you got a newline, then clear currentLine

currentLine = "";

}

} else if (c != '\r') { // if you got anything else but a carriage return character,

currentLine += c; // add it to the end of the currentLine

}

}

}

// Clear the header variable

header = "";

// Close the connection

client.stop();

Serial.println("Client disconnected.");

Serial.println("");

}

}
