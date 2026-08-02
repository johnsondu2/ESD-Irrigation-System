#include "iot.h"

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "config.h"
#include "system.h"
#include "tank.h"

// ThingsBoard Cloud connection info
static const char* TB_SERVER = "thingsboard.cloud";
static const int TB_PORT = 1883;  // switch to 8883 when we finalise evertyhing (encryption)

static WiFiClient espClient;
static PubSubClient client(espClient);

static unsigned long lastTelemetryPublish = 0;
static const unsigned long TELEMETRY_INTERVAL_MS = 30000;  // publish every 30s, tune as needed

// ————————————————————————————————————————————————————————————————————————

static void ConnectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

static void ReconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to ThingsBoard...");
    // The access token goes in the MQTT username slot; password stays empty
    if (client.connect("ESP32GardenController", TB_ACCESS_TOKEN, NULL)) {
      Serial.println("Connected to ThingsBoard");
    } else {
      Serial.print("Connect failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

static const char* StateToString(State s) {
  switch (s) {
    case IDLE:
      return "IDLE";
    case WATERING:
      return "WATERING";
    case FAULTY:
      return "FAULTY";
  }
  return "UNKNOWN";
}

// ——————————————————————————————————— public ———————————————————————————————————

void IoTSetup() {
  ConnectWiFi();
  client.setServer(TB_SERVER, TB_PORT);
}

void IoTLoop() {
  if (!client.connected()) {
    ReconnectMQTT();
  }
  client.loop();  // keeps the MQTT connection alive - must run every loop

  if (millis() - lastTelemetryPublish >= TELEMETRY_INTERVAL_MS) {
    lastTelemetryPublish = millis();
    IoTSendTelemetry();
  }
}

void IoTSendTelemetry() {
  StaticJsonDocument<256> telemetry;

  telemetry["deficit_mm"] = GetDeficitMm();
  telemetry["tank_empty"] = IsTankEmpty();
  telemetry["tank_level_percent"] = GetTankLevelPercent();
  telemetry["state"] = StateToString(GetState());
  telemetry["watering"] = (GetState() == WATERING);

  char buffer[256];
  serializeJson(telemetry, buffer);
  client.publish("v1/devices/me/telemetry", buffer);
}