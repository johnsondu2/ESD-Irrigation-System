#pragma once

// Call once in setup()
void IoTSetup();

// Call every SystemUpdate() iteration. Keeps WiFi/MQTT alive and publishes telemetry on its own internal timer
void IoTLoop();

// Publishes an immediate telemetry snapshot. IoTLoop() (helper) call if want to force an
// out-of-cycle update (e.g. right after a state change).
void IoTSendTelemetry();