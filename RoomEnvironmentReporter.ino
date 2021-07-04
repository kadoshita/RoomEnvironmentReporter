#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>



WebServer server(80);

void handleStatus()
{
  server.send(200, "application/json", "{\"status\":\"OK\"}");
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
    delay(500);
    digitalWrite(10, LOW);
    delay(500);
  }
  digitalWrite(10, LOW);

  server.on("/status", handleStatus);
  server.begin();

  M5.begin();
  M5.Lcd.print("Test");
}

void loop()
{
  server.handleClient();
}
