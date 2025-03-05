#define BLYNK_PRINT Serial
#include <Arduino.h>
#include <TinyGPSPlus.h>//https://github.com/mikalhart/TinyGPSPlus
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>
WidgetMap myMap(V0);

//sender phone number with country
char auth[] = "YicDy7Y4LYH09pWkMN4KZMNrr87w0ndZ"; // Blynk authentication key
const char* ssid     = "Thu Thao";
const char* password = "917664ab";
const char* SERVER_NAME = "http://esp8266gps.atwebpages.com/gpsdata.php";
String ESP32_API_KEY = "Ad5F10jkBM0";
unsigned long previousMillis = 0,times=0;
String sms_status,sender_number,received_date,msg;
bool newData=false;
long interval = 10000;
SoftwareSerial ss(D1, D2);
TinyGPSPlus gps;
 float flat, flon,val,val1;
 String sms1="";
 int gui = 0;
void setup() {
  delay(7000);
  Serial.begin(9600);
  ss.begin(9600);
  Serial.println("ss serial initialize");
  delay(1000);
  Blynk.begin(auth, ssid, password, "blynk-server.com", 8080);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
times = millis();
}
BLYNK_WRITE(V1) {
  gui = param.asInt();
}
void loop() {
  Blynk.run();

    static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
   printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat2(gps.location.lng(), gps.location.isValid(), 12, 6);
  Serial.println();
if(gui == 1){
  Blynk.email("GPS","VI TRI HIEN TAI LA: ", String("http://maps.google.com/maps?q=loc:")+String(flat)+String(",")+String(flon));
Serial.println("DA GU MAIL");
delay(1000);
}
if(millis() - times > 60000 && gps.encode(ss.read())>0){
     myMap.location(1, flat,flon,"GPS CUA TOI");
     delay(100);
     Serial.print("Latitude= "); 
    Serial.print(flat);
    Serial.println(" Longitude= "); 
    Serial.println(flon);
    delay(300);
    WiFiClient client;
    HTTPClient http;
    
    //domain name with path
    http.begin(client,SERVER_NAME);
    
    //Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    //----------------------------------------------
    //HTTP POST request data
    String gps_data;
    gps_data = "api_key="+ESP32_API_KEY;
    gps_data += String("&lat=")+String(flat);
    gps_data += String("&lng=")+String(flon);

    Serial.print("gps_data: ");
    Serial.println(gps_data);
    //----------------------------------------------

    //----------------------------------------------
    //Send HTTP POST Request and get response
    int httpResponseCode = http.POST(gps_data);
    String httpResponseString = http.getString();
    
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println(httpResponseString);
    }
    //----------------------------------------------
    else {
      Serial.print("Error on HTTP request - Error code: ");
      Serial.println(httpResponseCode);
      Serial.println(httpResponseString);
    }
    //----------------------------------------------
    // Free resources
    http.end();
    times = millis();

// This custom version of delay() ensures that the gps object
// is being "fed".
}
}
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available()){
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
      flat=val;
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}
static void printFloat2(float val1, bool valid1, int len1, int prec1)
{
  if (!valid1)
  {
    while (len1-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val1, prec1);
      flon=val1;
    int vi = abs((int)val1);
    int flen = prec1 + (val1 < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len1; ++i)
      Serial.print(' ');
      
  }
  smartDelay(0);
}
 
    
