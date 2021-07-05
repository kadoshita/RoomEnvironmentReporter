#include <M5StickC.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>

BLEScan *BLEScanner;
BLEUUID serviceUUID = BLEUUID("cba20d00-224d-11e6-9fb8-0002a5d5c51b");
BLEUUID serviceDataUUID = BLEUUID("00000d00-0000-1000-8000-00805f9b34fb");

Adafruit_BMP280 bmp280;
int meterBatteryLevel = 0;
float meterTemperature = 0.0;
int meterHumidity = 0;

String inputString = "";
bool stringComplete = false;

class AdvertisedDeviceCallback : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice device)
  {
    if (
        !device.haveServiceUUID() ||
        !device.getServiceUUID().equals(serviceUUID) ||
        !device.haveServiceData() ||
        !device.getServiceDataUUID().equals(serviceDataUUID))
    {
      return;
    }

    std::string str = device.getServiceData();
    const char *data = str.c_str();
    int tmpBatteryLevel = data[2] & 0b01111111;
    float tmpMeterTemperature = (data[3] & 0b00001111) / 10.0 + (data[4] & 0b01111111);
    int tmpMeterHumidity = data[5] & 0b01111111;
    if (tmpBatteryLevel <= 0 || tmpMeterHumidity <= 0 || (meterTemperature != 0.0 && abs(tmpMeterTemperature - meterTemperature) > 10.0))
    {
      return;
    }
    meterBatteryLevel = tmpBatteryLevel;
    bool isBelowZero = !(data[4] & 0b10000000);
    meterTemperature = tmpMeterTemperature;
    if (isBelowZero)
    {
      meterTemperature = -meterTemperature;
    }

    meterHumidity = tmpMeterHumidity;
  }
};

void sendData()
{
  char data[255] = {0};
  float temperature = bmp280.readTemperature();
  float pressure = bmp280.readPressure() / 100;
  float battery_level = M5.Axp.GetVbatData() * 1.1 / 1000;
  sprintf(data, "{\"temperature\":%f,\"pressure\":%f,\"battery_level\":%f,\"meter\":{\"temperature\":%f,\"humidity\":%d,\"battery_level\":%d}}", temperature, pressure, battery_level, meterTemperature, meterHumidity, meterBatteryLevel);
  Serial.println(data);
}
void setup()
{
  Serial.begin(115200);
  inputString.reserve(255);
  pinMode(10, OUTPUT);

  while (!bmp280.begin(0x77))
  {
    digitalWrite(10, HIGH);
    delay(200);
    digitalWrite(10, LOW);
    delay(200);
  }

  BLEDevice::init("");
  BLEScanner = BLEDevice::getScan();
  BLEScanner->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallback(), true);
  BLEScanner->setActiveScan(true);
  BLEScanner->setInterval(100);
  BLEScanner->setWindow(99);

  digitalWrite(10, HIGH);
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Axp.ScreenBreath(0);
}

void loop()
{
  BLEScanResults result = BLEScanner->start(5, false);
  BLEScanner->clearResults();
  sendData();
  delay(60000);
}