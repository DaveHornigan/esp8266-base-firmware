#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#define viewIpDelay 1000 * 60 * 1 // 1 minute

unsigned long lastViewedIpTime = millis();

void printIpToSerial();
bool isNeedPrintIp();

void setup()
{
  Serial.begin(115200);

  WiFiManager wifiManager;
  if (!wifiManager.autoConnect())
  {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
  printIpToSerial();
}

void loop()
{
  if (isNeedPrintIp())
  {
    printIpToSerial();
  }
}

void printIpToSerial()
{
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  lastViewedIpTime = millis();
}

bool isNeedPrintIp()
{
  return WiFi.status() == WL_CONNECTED && millis() - lastViewedIpTime > viewIpDelay;
}