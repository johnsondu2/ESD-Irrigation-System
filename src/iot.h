#pragma once

#include "bed.h"

extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;
extern const char* TB_SERVER;
extern const char* TB_ACCESS_TOKEN;

// Call once in setup()
void IoTSetup();

// Call every loop() iteration. Keeps WiFi/MQTT alive
void IoTLoop();

// Call whenever we have new sensor readings to send
void IoTSendTelemetry(Bed beds[], int numBeds);

void IoTSetBeds(Bed beds[], int numBeds);
