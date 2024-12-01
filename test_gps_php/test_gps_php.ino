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
char auth[] = "YicDy7Y4LYH09pWkMN4KZMNrr87w0ndZ"; //Mã kết nối với blynk
const char* ssid     = "Thu Thao";   //Tên Wifi
const char* password = "917664ab";    //Mk Wifi
const char* SERVER_NAME = "http://esp8266gps.atwebpages.com/gpsdata.php";  //Địa chỉ Server
String ESP32_API_KEY = "Ad5F10jkBM0";   //Key Server bảo mật
unsigned long previousMillis = 0,times=0;   //Biến thười gian
String sms_status,sender_number,received_date,msg;  //Biến String
bool newData=false;
long interval = 10000;    //
SoftwareSerial ss(D1, D2);   //Thiết lập Serial ảo là chân D1-RX, D2-TX
TinyGPSPlus gps;             //Set tên cho module gps
 float flat, flon,val,val1;  //Biến float
 String sms1="";
 int gui = 0;
void setup() {           //Hàm cài đặt i/o chỉ chạy 1 lần khi khởi động
  delay(7000);          //Chờ 7 giây để các thiết bị khởi động hoàn tất
  Serial.begin(9600);   //Tốc độ liên kết với máy tính
  ss.begin(9600);       //Set tốc độ liên kết module gps với esp8266
  Serial.println("ss serial initialize");
  delay(1000);
  Blynk.begin(auth, ssid, password, "blynk-server.com", 8080);//Khởi động Blynk
  WiFi.begin(ssid, password);  //Kết nối wifi
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { //Nếu ko kết nối đc thì chừo kết nối
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());//Hiển thị ip 
times = millis();
}
BLYNK_WRITE(V1) {
  gui = param.asInt();
}
void loop() {
  Blynk.run();//Khởi động blynk

    static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;//Ghi giá trị ban đầu
   printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);//Đọc dữ liệu từ gps
  printFloat2(gps.location.lng(), gps.location.isValid(), 12, 6);
  Serial.println();
if(gui == 1){
  Blynk.email("GPS","VI TRI HIEN TAI LA: ", String("http://maps.google.com/maps?q=loc:")+String(flat)+String(",")+String(flon));//Gửi mail nếu biến gửi =1 
Serial.println("DA GU MAIL");
delay(1000);
}
if(millis() - times > 60000 && gps.encode(ss.read())>0){//Nếu thời gian lớn hơn 1 phút thì gửi lên server 1 lần
     myMap.location(1, flat,flon,"GPS CUA TOI"); //Gửi vị trí gps lên Blynk
     delay(100);
     Serial.print("Latitude= "); 
    Serial.print(flat);
    Serial.println(" Longitude= "); 
    Serial.println(flon);
    delay(300);
    WiFiClient client;
    HTTPClient http;
    
    //domain name with path
    http.begin(client,SERVER_NAME);     //Khởi động server
    
    //Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    //----------------------------------------------
    //HTTP POST request data
    String gps_data;                            
    gps_data = "api_key="+ESP32_API_KEY;        //Gửi dữ liệu lên server
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
    http.end();            //Kết thúc gửi dữ liệu
    times = millis();

// This custom version of delay() ensures that the gps object
// is being "fed".
}
}
static void smartDelay(unsigned long ms)    //Hàm đọc dữ liệu từ gps
{
  unsigned long start = millis();
  do 
  {
    while (ss.available()){
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)//Hàm chuyển đổi dữ liệu float
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
 
    
