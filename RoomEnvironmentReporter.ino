#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>



WebServer server(80);

Adafruit_BMP280 bmp280;

void handleStatus()
{
  char body[255] = {0};
  float temperature = bmp280.readTemperature();
  float pressure = bmp280.readPressure() / 100;
  float battery_level = M5.Axp.GetVbatData() * 1.1 / 1000;
  sprintf(body, "{\"status\":\"OK\",\"temperature\":%f,\"pressure\":%f,\"battery_level\":%f}", temperature, pressure, battery_level);
  server.send(200, "application/json", body);
}
void setup()
{
  pinMode(10, OUTPUT);
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    digitalWrite(10, HIGH);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(10, HIGH);
    delay(200);
    digitalWrite(10, LOW);
    delay(200);
  }

  while (!bmp280.begin(0x77))
  {
    digitalWrite(10, HIGH);
    delay(200);
    digitalWrite(10, LOW);
    delay(200);
  }

  digitalWrite(10, HIGH);
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Axp.ScreenBreath(0);

  server.on("/status", handleStatus);
  server.begin();
}

void loop()
{
  server.handleClient();
}
