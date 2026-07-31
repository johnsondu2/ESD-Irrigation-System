#pragma once

// Call once in setup()
void IoTSetup();

// Call every loop() iteration. Keeps WiFi/MQTT alive
void IoTLoop();

// Call whenever we have new sensor readings to send
void IoTSendTelemetry(Bed beds[], int numBeds);

void IoTSetBeds(Bed beds[], int numBeds);