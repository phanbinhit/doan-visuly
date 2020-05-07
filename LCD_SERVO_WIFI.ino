#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
int i = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define SERVO_PIN D4

Servo gServo;
#include <NTPClient.h>
// change next line to use with another board/shield
#include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>

const char *ssid     = "FPT Hoa";
const char *password = "1234567890";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
bool clearData(const char* filename) {
  SPIFFS.begin();
  File f = SPIFFS.open(String("/") + filename, "w");
  if (!f) {
    f.close();
    return false;
  }
  else {
    f.close();
    return true;
  }
}
bool saveData(const char* filename, const char* content, uint16_t len) {
  SPIFFS.begin();
  File f = SPIFFS.open(String("/") + filename , "a");
  if (!f) {
    f.close();
    return false;
  }
  else {
    f.write(content, len);
    f.close();
    return true;
  }
}


String readData(const char* filename) {
  SPIFFS.begin();
  File f = SPIFFS.open(String("/") + filename , "r");
  String ret = f.readString();
  f.close();
  return ret;
}

ESP8266WebServer sv(80);

void setup()
{
  gServo.attach(SERVO_PIN);
  lcd.begin (16, 2); // for 16 x 2 LCD module

  lcd.setBacklight(LOW);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("FPT Hoa T2", "12345678");

  WiFi.begin("FPT Hoa", "1234567890");
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
    delay(500);
  Serial.begin(115200);
  Serial.println(WiFi.localIP());
  sv.on("/", HTTP_GET, [] {
    sv.send(200, "text/html", readData("index.html"));
    Serial.println("OKOK");
  });

  sv.on("/update", HTTP_ANY,  [] {
    sv.send(200, "text/html",
    "<meta charset = 'utf-8'>"
    "<html>"
    "<head>"
    "<title>"
    "Update Page"
    "</title>"
    "</head>"
    "<body>"
    "<form method = 'POST' action = '/update' enctype = 'multipart/form-data'>"
    "<input type = 'file' name = 'chon File' >"
    "<input type = 'submit' value = 'Gửi File' >"
    "</form>"
    "</body>"
    "</html>" );
  }, [] {

    HTTPUpload& file = sv.upload();
    if (file.status == UPLOAD_FILE_START) {
      clearData("index.html");
    }
    else if (file.status == UPLOAD_FILE_WRITE) {
      saveData("index.html", (const char*) file.buf, file.currentSize);
    }
  }
       );


  sv.begin();

}



void loop()
{
  sv.handleClient();


  // hien thi time len LCD
  
  lcd.setCursor(0, 0);
  int hour = (timeClient.getHours() + 7) % 24;
  int minute = timeClient.getMinutes();
  int second = timeClient.getSeconds();
  lcd.print("Time: " + String(hour) + ":" + String(minute) + ":" + String(second));
  timeClient.update();

  

  // dieu khien servo
  gServo.write(0);
  for (int pos = 0; pos < 180; pos += 1) {

    gServo.write(pos);
    delay(3);
  }
  delay(1000 - 180 * 3);
}
