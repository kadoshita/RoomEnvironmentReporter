#include "time.h"
#include <M5StickC.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>

#define USE_SERIAL

#define TEST 0
#define TEMPERATURE 1
#define PRESSURE 2
#define BATTERY_LEVEL 3

const char *WIFI_SSID = "";
const char *WIFI_PASS = "";
const char *NTP_SERVER = "ntp.nict.jp";
const char *HTTP_URL = "http://example.com:8086/write?db=<dbname>&u=<username>&p=<password>";

Adafruit_BMP280 bmp280;

int post_data(int mode, float value);
void display_clock(int count);

void setup()
{
  M5.begin();
  M5.Axp.ScreenBreath(10);
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextSize(4);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.fillScreen(BLACK);

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
#ifdef USE_SERIAL
  Serial.println("WiFi Connecting...");
#endif
  Wire.begin();
  while (!bmp280.begin(0x77))
  {
#ifdef USE_SERIAL
    Serial.println("BMP280 init fail");
#endif
    delay(500);
  }
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
#ifdef USE_SERIAL
  Serial.println("WiFi Connected");
  Serial.print("IP:");
  Serial.println(WiFi.localIP());
#endif
  configTime(9 * 3600, 0, NTP_SERVER);
}

void loop()
{
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, M5.Lcd.height() * 0.3);
  M5.Lcd.println("uploading...");
  float temperature = bmp280.readTemperature();
  float pressure = bmp280.readPressure() / 100;
  float battery_level = M5.Axp.GetVbatData() * 1.1 / 1000;
#ifdef USE_SERIAL
  Serial.printf("temperature:%f\tpressure:%f\tbattery_level:%f\r\n", temperature, pressure, battery_level);
#endif
  int status=0;
  status+=post_data(TEMPERATURE, temperature);
  status+=post_data(PRESSURE, pressure);
  status+=post_data(BATTERY_LEVEL, battery_level);
  if(status==0){
    M5.Lcd.println("OK");
    delay(500);
  }else{
    M5.Lcd.println("ERROR");
    delay(500);
  }
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("temperature\r\n");
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(0, M5.Lcd.height() * 0.3);
  M5.Lcd.printf(" %4.1f\r\n", temperature);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(M5.Lcd.width() * 0.75, M5.Lcd.height() * 0.75);
  M5.Lcd.println("'C");
  delay(5000);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("pressure\r\n");
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(0, M5.Lcd.height() * 0.3);
  M5.Lcd.printf("%4.1f\r\n", pressure);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(M5.Lcd.width() * 0.75, M5.Lcd.height() * 0.75);
  M5.Lcd.println("hPa");
  delay(5000);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("battery_level\r\n");
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(0, M5.Lcd.height() * 0.3);
  M5.Lcd.printf(" %4.2f\r\n", battery_level);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(M5.Lcd.width() * 0.75, M5.Lcd.height() * 0.75);
  M5.Lcd.println("V");
  delay(5000);
  display_clock(5);
}

int post_data(int mode, float value)
{
  char data[255] = {0};
  switch (mode)
  {
  case TEST:
    sprintf(data, "test value=%f", value);
    break;
  case TEMPERATURE:
    sprintf(data, "temperature value=%f", value);
    break;
  case PRESSURE:
    sprintf(data, "pressure value=%f", value);
    break;
  case BATTERY_LEVEL:
    sprintf(data, "battery_level value=%f", value);
    break;
  default:
    break;
  }

  HTTPClient http;
  http.begin(HTTP_URL);
  int status = http.POST((uint8_t *)data, strlen(data));
#ifdef USE_SERIAL
  Serial.println(status);
  if (status == 204)
  {
    Serial.println(http.getString());
    status = 0;
  }
  else
  {
    WiFi.reconnect();
#ifdef USE_SERIAL
    Serial.println("WiFi ReConnected");
    Serial.print("IP:");
    Serial.println(WiFi.localIP());
#endif
    status = 1;
  }
#endif
  http.end();
  return status;
}
void display_clock(int count)
{
  struct tm timeinfo;
  int i = 0;
  for (i = 0; i < count; i++)
  {
    if (getLocalTime(&timeinfo))
    {
      int yer = timeinfo.tm_year + 1900;
      int mon = timeinfo.tm_mon + 1;
      int day = timeinfo.tm_mday;
      int hh = timeinfo.tm_hour;
      int mm = timeinfo.tm_min;
      int ss = timeinfo.tm_sec;
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.setTextSize(2);
      M5.Lcd.printf("%04d/%02d/%02d\r\n", yer, mon, day);
      M5.Lcd.setCursor(0, M5.Lcd.height() * 0.3);
      M5.Lcd.setTextSize(3);
      M5.Lcd.printf("%02d:%02d:%02d\r\n", hh, mm, ss);
    }
    delay(1000);
  }
}
