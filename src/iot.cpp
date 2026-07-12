#include "iot.h"

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "config.h"

// WiFi credentials
const char* WIFI_SSID = "wifi_name_here";
const char* WIFI_PASSWORD = "wifi_password_here";

// ThingsBoard connection info
const char* TB_SERVER = "thingsboard.cloud";
const char* TB_ACCESS_TOKEN = "token_here";

WiFiClient espClient;
PubSubClient client(espClient);  // library has own funcitons

Bed* bedsPtr = nullptr;
int numBedsGlobal = 0;

// –——––––––––––––––––––––––––––––––––––

void ConnectWiFi() {  // OK
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void ReconnectMQTT() {  // OK
  while (!client.connected()) {
    Serial.println("Connecting to ThingsBoard...");
    if (client.connect("ESP32Device", TB_ACCESS_TOKEN, NULL)) {
      Serial.println("Connected to ThingsBoard");
      client.subscribe("v1/devices/me/rpc/request/+");  // listen for commands
    } else {
      delay(2000);
    }
  }
}

// TO DO: implement handling of user commands (would need to update fsm logic)
void MQTTCallback(char* topic, byte* payload, unsigned int length) {
  if (bedsPtr == nullptr) return;

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload, length);

  String method = doc["method"];
  if (method == "setValve") {
    int bedIndex = doc["params"]["bed"];
    String state = doc["params"]["state"];

    if (bedIndex >= 0 && bedIndex < numBedsGlobal) {
      if (state == "open") {
        bedsPtr[bedIndex].OpenValves();
      } else {
        bedsPtr[bedIndex].CloseValves();
      }
    }
  }
}

// ——————————————————————————————— public —————————————————————————————————

void IoTSetup() {
  ConnectWiFi();
  client.setServer(TB_SERVER, 1883);
  client.setCallback(MQTTCallback);
}

void IoTLoop() {
  if (!client.connected()) {
    ReconnectMQTT();
  }
  client.loop();
}

void IoTSendTelemetry(Bed beds[], int numBeds) {
  StaticJsonDocument<512> telemetry;

  for (int i = 0; i < numBeds; i++) {
    String key = "bed" + String(i) + "_moisture";
    telemetry[key] = beds[i].GetSensorReading();
  }

  char buffer[512];
  serializeJson(telemetry, buffer);
  client.publish("v1/devices/me/telemetry", buffer);
}

void IoTSetBeds(Bed beds[], int numBeds) {
  bedsPtr = beds;
  numBedsGlobal = numBeds;
}