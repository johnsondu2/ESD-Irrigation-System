#include "iot.h"

#include <ArduinoJson.h>

#include "config.h"

// ThingsBoard Cloud connection info
static const char* TB_SERVER = "thingsboard.cloud";
static const int TB_PORT = 1883;  // switch to 8883 when we finalise evertyhing (encryption)

static const unsigned long TELEMETRY_INTERVAL_MS = 3000;  // publish every 3s, tune as needed

// ——————————————————————————————————— public ———————————————————————————————————

IoT::IoT(Tank& tank)
    : _tank(tank),
      _wifiClient(),
      _mqttClient(_wifiClient),
      _lastTelemetryPublishMs(0) {}

void IoT::begin() {
  connectWiFi();
  _mqttClient.setServer(TB_SERVER, TB_PORT);
}

void IoT::loop(State state, float deficitMm) {
  if (!_mqttClient.connected()) {
    reconnectMQTT();
  }
  _mqttClient.loop();  // keeps the MQTT connection alive - must run every loop

  if (millis() - _lastTelemetryPublishMs >= TELEMETRY_INTERVAL_MS) {
    _lastTelemetryPublishMs = millis();
    sendTelemetry(state, deficitMm);
  }
}

void IoT::sendTelemetry(State state, float deficitMm) {
  JsonDocument telemetry;

  telemetry["deficit_mm"] = deficitMm;
  telemetry["tank_empty"] = _tank.IsTankEmpty();
  telemetry["tank_level_percent"] = _tank.GetTankLevelPercent();
  telemetry["state"] = stateToString(state);
  telemetry["watering"] = (state == PUMPING);

  char buffer[256];
  serializeJson(telemetry, buffer);
  _mqttClient.publish("v1/devices/me/telemetry", buffer);
}

// ——————————————————————————————————— private ——————————————————————————————————

void IoT::connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void IoT::reconnectMQTT() {
  while (!_mqttClient.connected()) {
    Serial.println("Connecting to ThingsBoard...");
    // The access token goes in the MQTT username slot; password stays empty
    if (_mqttClient.connect("ESP32GardenController", TB_ACCESS_TOKEN, NULL)) {
      Serial.println("Connected to ThingsBoard");
    } else {
      Serial.print("Connect failed, rc=");
      Serial.println(_mqttClient.state());
      delay(2000);
    }
  }
}

const char* IoT::stateToString(State s) {
  switch (s) {
    case IDLE:
      return "IDLE";
    case PUMPING:
      return "PUMPING";
    case NO_WATER:
      return "NO_WATER";
    case FAULTY:
      return "FAULTY";
  }
  return "UNKNOWN";
}
