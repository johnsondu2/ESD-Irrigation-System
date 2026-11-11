#include "ultrasonic_tank.h"

#include <Arduino.h>
#include <esp_now.h>
#include <algorithm>  // Required for std::max and std::min

// Tank state, owned here and updated whenever a reading arrives from the tank ESP32
static float tankDistanceCm = -1.0f;      // last distance received, in cm; -1 = unknown / transmitter-reported fault

// Triggers the ultrasonic ranger and returns the measured distance in cm,
// or -1.0f if no echo was received (sensor fault/out of range)
float ReadTankDistanceCm() {

  // Ensure trigger pin is low
  digitalWrite(ULTRA_TRIG, LOW);
  delayMicroseconds(2);

  // 10 us pulse to the trigger pin
  digitalWrite(ULTRA_TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(ULTRA_TRIG, LOW);

  // Read echo pulse duration with a 30,000us max timeout as a fail-safe
  long duration = pulseIn(ULTRA_ECHO, HIGH, 30000);

  // guard
  if (duration == 0) {
    Serial.println("ultra: Error / Timeout");
    return -1.0f;
  }

  // calc distance, using speed of air = 343 m/s, and dividing by 2 for the round trip
  float dist = (duration * 0.0343f) / 2.0f;

  Serial.print("ultra: ");
  Serial.print(dist);
  Serial.println(" cm");

  return dist;
}


/* old implementation below:
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
