#include "receiver.h"

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Tank state, owned here and updated whenever a reading arrives from the tank ESP32
static float tankDistanceCm = -1.0f;      // last distance received, in cm; -1 = unknown / transmitter-reported fault
static unsigned long lastReceivedMs = 0;  // millis() timestamp of the last reading received
static bool everReceived = false;         // true once at least one reading has arrived since boot

// Callback function updated for ESP32 Core v3.x
static void OnDataRecv(const esp_now_recv_info *recv_info, const uint8_t *incomingData, int len) {
  Serial.print("Tank data received from: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", recv_info->src_addr[i]);
    if (i < 5) Serial.print(":");
  }

  // Reconstruct the 4-byte 'long' integer distance data from the incoming byte array
  long receivedDistance;
  memcpy(&receivedDistance, incomingData, sizeof(receivedDistance));

  Serial.print(" | Distance: ");

  // Process the measurement values or error flags sent by the transmitter
  if (receivedDistance == -1) {
    Serial.println("Error (Sensor line high/busy)");
  } else if (receivedDistance == -2) {
    Serial.println("Out of range (>4.5m)");
  } else if (receivedDistance == -3) {
    Serial.println("Error (Sensor disconnected/timeout)");
  } else {
    Serial.print(receivedDistance);
    Serial.println(" cm");
  }

  // Any negative sentinel from the transmitter collapses to "unknown" here; IsTankEmpty()/GetTankLevelPercent()
  // fail safe on that
  tankDistanceCm = (receivedDistance >= 0) ? (float)receivedDistance : -1.0f;
  lastReceivedMs = millis();
  everReceived = true;
}

void ReceiverBegin() {
  WiFi.mode(WIFI_STA);
  WiFi.begin();  // Wake up radio

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Receiver online and listening for tank data...");
}

float GetTankDistanceCm() { return tankDistanceCm; }

bool IsTankEmpty() {
  // Fail-safe: no reading yet, a reported sensor fault, or a stale link all count as "empty"
  if (!everReceived || tankDistanceCm < 0 || (millis() - lastReceivedMs) >= TANK_DATA_TIMEOUT_MS) {
    return true;
  }

  return tankDistanceCm >= TANK_EMPTY_DISTANCE_CM;
}

float GetTankLevelPercent() {
  if (!everReceived || tankDistanceCm < 0 || (millis() - lastReceivedMs) >= TANK_DATA_TIMEOUT_MS) {
    return -1.0f;  // unknown
  }

  float percent = (TANK_EMPTY_DISTANCE_CM - tankDistanceCm) / (TANK_EMPTY_DISTANCE_CM - TANK_FULL_DISTANCE_CM) * 100.0f;

  // Clamp: a reading slightly past either calibration point (sensor noise, tank
  // slightly overfilled, etc.) shouldn't report an out-of-range percentage.
  percent = std::max(0.0f, std::min(100.0f, percent));

  return percent;
}
