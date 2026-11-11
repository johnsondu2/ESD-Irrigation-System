#pragma once

// Tank geometry calibration (same sensor/tank as before; now read by a separate ESP32 and sent over ESP-NOW)
#define TANK_EMPTY_DISTANCE_CM 116.0f  // placeholder: distance (cm) from sensor to water surface that counts as "empty"; needs calibration against actual tank geometry
#define TANK_FULL_DISTANCE_CM 5.0f     // guess

#define TANK_DATA_TIMEOUT_MS 60000UL  // if no reading has arrived from the tank ESP32 within this window, treat the tank as empty (fail-safe)

// Call once from SystemBegin(): brings up ESP-NOW and starts listening for tank readings
void ReceiverBegin();

// Last distance reported by the tank ESP32, in cm (-1 if never received, or the transmitter reported a sensor fault)
float GetTankDistanceCm();

// Tank fill level as a percentage, derived from the last received distance (-1 if unknown/stale/fault)
float GetTankLevelPercent();

// True if the tank is empty, no reading has ever arrived, the last reading reported a sensor fault,
// or the link has gone stale (no data within TANK_DATA_TIMEOUT_MS) - fails safe in all of those cases
bool IsTankEmpty();
