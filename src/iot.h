#pragma once

#include <PubSubClient.h>
#include <WiFi.h>

#include "system.h"
#include "tank.h"

/*
 * Handles the WiFi connection and ThingsBoard (MQTT) telemetry.
 * WiFi/ThingsBoard credentials come from config.h (gitignored).
 *
 * Usage:
 *   IoT iot(tank);
 *   void setup() { iot.begin(); }
 *   void loop()  { iot.loop(state, deficitMm); }
 */

class IoT {
public:
  // tank to report % level in the telemetry
  IoT(Tank& tank);

  // Connects to WiFi and configures the MQTT server. Call once in setup().
  void begin();

  // to call every loop interaction
  void loop(State state, float deficitMm);

  void sendTelemetry(State state, float deficitMm);

private:
  void connectWiFi();
  void reconnectMQTT();
  static const char* stateToString(State s);

  Tank& _tank;

  WiFiClient _wifiClient;  // must be declared before _mqttClient (it's used to construct it)
  PubSubClient _mqttClient;

  unsigned long _lastTelemetryPublishMs;
};
