#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <DNSServer.h>
#include <PubSubClient.h>
#include "WiFiManager.h"

#define VIEW_IP_DELAY 1000 * 60 * 1 // 1 minute

#define MQTT_MESSAGE_DELAY 2000
#define MQTT_HOST "192.168.0.10"
#define MQTT_PORT 1883
#define MQTT_USER "mqtt"
#define MQTT_PASS "44A4E3T#BhNnNF4%e9fv"

#define MESSAGE_BUFFER_SIZE	(1024)

char message[MESSAGE_BUFFER_SIZE];
uint64 value = 0;

WiFiClient wiFiClient;
PubSubClient client(wiFiClient);

uint64 lastMqttMessage = 0;
uint64 lastViewedIpTime = millis();

void printIpToSerial();
bool isNeedPrintIp();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFiManager wifiManager;
  if (!wifiManager.autoConnect())
  {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
  printIpToSerial();

  randomSeed(micros());
  printIpToSerial();
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
}

void loop()
{
  uint64 now = millis();
  if (isNeedPrintIp())
  {
    printIpToSerial();
  }
  if (!client.connected() && WiFi.status() == WL_CONNECTED) {
    reconnect();
  }
  client.loop();

  if (now - lastMqttMessage > MQTT_MESSAGE_DELAY) {
    lastMqttMessage = now;
    ++value;
    snprintf (message, MESSAGE_BUFFER_SIZE, "hello world #%llu", value);
    Serial.print("Publish message: ");
    Serial.println(message);
    client.publish("test/outTopic", message);
  }
}

void printIpToSerial()
{
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
  lastViewedIpTime = millis();
}

bool isNeedPrintIp()
{
  return WiFi.status() == WL_CONNECTED && millis() - lastViewedIpTime > VIEW_IP_DELAY;
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.printf("Message arrived [%s]: ", topic);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
    while (!client.connected()) {
        Serial.println("Attempting MQTT connection...");
        String clientId = "ESP8266Client-";
        clientId += String(EspClass::getChipId());
        if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
            Serial.printf("Connected with client ID: %s\n", clientId.c_str());
            client.publish("test/outTopic", "hello world");
            client.subscribe("test/inTopic");
        } else {
            Serial.printf("Failed, rc=%d try again in 5 seconds\n", client.state());
            delay(5000);
        }
    }
}