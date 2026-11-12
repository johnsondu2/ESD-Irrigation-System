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
 *   void loop()  { iot.loop(); }
 */
class IoT {
public:
  // tank: the tank whose level/empty state is reported in telemetry
  IoT(Tank& tank);

  // Connects to WiFi and configures the MQTT server. Call once in setup().
  void begin();

  // Call every loop iteration. Keeps the MQTT connection alive and publishes
  // telemetry on its own internal timer.
  void loop();

  // Publishes a telemetry snapshot immediately. loop() calls this on its timer;
  // call it directly to force an out-of-cycle update (e.g. right after a state change).
  void sendTelemetry();

private:
  void connectWiFi();
  void reconnectMQTT();
  static const char* stateToString(State s);

  Tank& _tank;

  WiFiClient _wifiClient;  // must be declared before _mqttClient (it's used to construct it)
  PubSubClient _mqttClient;

  unsigned long _lastTelemetryPublishMs;
};
