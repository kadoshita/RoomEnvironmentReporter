#include <M5StickC.h>
#include <WiFi.h>



void setup()
{
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    digitalWrite(10, HIGH);
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(10, HIGH);
    delay(500);
    digitalWrite(10, LOW);
    delay(500);
  }

  M5.begin();
  M5.Lcd.print("Test");
}

void loop()
{
  delay(10 * 1000);
}
