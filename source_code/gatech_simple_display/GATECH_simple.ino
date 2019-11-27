/*  B7971 DUSTOFF IoT BOARD - Halloween Display for BAKER
 *  Date Modified: 1 Sep 19
 *  Developer: Mahdi Al-Husseini, mahdi07@msn.com
 *  
 *  Note: This project uses an Espressif 8266 NodeMCU LUA uC board, allowing for internet connectivity. 
 *  It is required you set the ssid and password variables appropriately, and your lon and lat if intending on using METAR/TAF (for now).
 *  License (Beerware): This code is open to the public but you buy me a beer if you use this and we meet someday
 */

#include <ESP8266WiFi.h>  // standard 8266 WiFi connectivity (https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html)
#include <WiFiClientSecure.h>  // supports secure connections using TLS (SSL) (https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure)
#include <WiFiUdp.h> // allows for sending and recieving UDP packets (https://www.arduino.cc/en/Reference/WiFiUDPConstructor)

// #include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>

/* For SparkFun IR Remote */ 
const uint16_t BUTTON_POWER = 0x629D;
const uint16_t BUTTON_A = 0x22DD;
const uint16_t BUTTON_B = 0x02FD;
const uint16_t BUTTON_C = 0xC23D;
const uint16_t BUTTON_UP = 0x9867;
const uint16_t BUTTON_DOWN = 0x38C7;
const uint16_t BUTTON_LEFT = 0x30CF;
const uint16_t BUTTON_RIGHT = 0x7A85;
const uint16_t BUTTON_CIRCLE = 0x18E7;

IRrecv irrecv(4); // Receive on pin 4
decode_results results;
uint16_t lastCode = 0; // This keeps track of the last code RX'd
String scrollText = "HAPPY HALLOWEEN FROM BAKER"; // default

/* WiFi */
const char* ssid = "ALI1224";
const char* password = "Ali#1224";

/* National Weather Service (NWS) API */
const char* host = "www.aviationweather.gov";

// Your current location; adjust as needed. This is currently hardcoded, but an update in the future will allow automatic triangulation of the 8266 location using LocationAPI from Unwired Labs.
String lon = "33.738328";
String lat = "-84.707237";

/* TAF is Terminal Aerodrome Forecast: https://en.wikipedia.org/wiki/Terminal_aerodrome_forecast
 * METAR is Meteorological Terminal Aviation Routine [Weather Report]: https://en.wikipedia.org/wiki/METAR
 * Guidance for designing url_METAR requests: https://www.aviationweather.gov/dataserver/example?datatype=metar 
 * Guidance for designing url_TAF requests: https://www.aviationweather.gov/dataserver/example?datatype=metar 
 * url_METAR: currently set to retrieve the most recent METAR in the past three hours within 20 statue miles
 * url_TAF: currently set to retrieve the most recent RAF in the past three hours within 20 statue miles
 */
 
String url_METAR = "/adds/dataserver_current/httpparam?dataSource=metars&requestType=retrieve&format=xml&radialDistance=20;" + lat + "," + lon + "&hoursBeforeNow=3&mostRecent=true&&fields=raw_text";
String url_TAF = "/adds/dataserver_current/httpparam?dataSource=tafs&requestType=retrieve&format=xml&radialDistance=20;" + lat + "," + lon + "&hoursBeforeNow=3&mostRecent=true&&fields=raw_text";
const int httpsPort = 443;
BearSSL::WiFiClientSecure client;

/* Configuration */
int pollDelay = 20000; //time between each retrieval
int retryWifiDelay = 5000;
const int API_TIMEOUT = 10000;
unsigned long time_now = 0;

// 7971 Smart Sockets
String submission;
String instance;
String wifi_status;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();  // Start the receiver
  // Serial.begin(115200);
  // {$B7F...}, {$B7E...}, and {$B7S...} are standard configurations for B7971 text display. This can be adjusted as desired; reference the smart_socket guide pdf. 
  Serial.println("$B7F0000000<cr>");
  Serial.println("$B7E0000000<cr>");
  Serial.println("$B7S0000000<cr>");
  // Retain "$B7M" and "<cr>, but replace "MEDEVAC" as desired for start-up display. 
  Serial.println("$B7MGATECH<cr>");
  // scroll("MEDEVAC");
  // connectToWifi();
  // client.setInsecure();
  // client.setTimeout(API_TIMEOUT);
}
void loop() {
}

/* Scroll text across B7971s */
void scroll(String data) {
  // Uppercase the input string
  data = "       " + data + "       ";
  data.toUpperCase();
  
  for (int x = 0; x < data.length() - 6; x++) {
    instance = data.substring(x, x+7);
    submission = "$B7M" + instance + "<cr>";
    Serial.println(submission);
    // Adjust delay speed for fast or slow scroll
    delay(300);
  }
}
