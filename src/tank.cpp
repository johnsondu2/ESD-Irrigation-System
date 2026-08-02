#include "tank.h"

#include <Arduino.h>

#include <algorithm>  // Required for std::max and std::min

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

  // Read echo pulse duration with a 30,000us max timeout to prevent ESP32 lockups
  long duration = pulseIn(ULTRA_ECHO, HIGH, 30000);

  // guard
  if (duration == 0) {
    Serial.println("ultra: Error / Timeout");
    return -1.0f;
  }

  // calc distance
  float dist = (duration * 0.0343f) / 2.0f;

  Serial.print("ultra: ");
  Serial.print(dist);
  Serial.println(" cm");

  return dist;
}

// Returns true if the tank is empty
bool IsTankEmpty() {
  return false;  // ADDED JUST FOR TESTING
  float distanceCm = ReadTankDistanceCm();
  if (distanceCm < 0.0f) {
    return true;  // Safety default: Treat a broken sensor as an empty tank to prevent dry-running pump
  }

  return distanceCm >= TANK_EMPTY_DISTANCE_CM;
}

float GetTankLevelPercent() {
  float distanceCm = ReadTankDistanceCm();

  // Guard against sensor faults or erratic bouncing echoes
  if (distanceCm < 0.0f || distanceCm > (TANK_EMPTY_DISTANCE_CM + 50.0f)) {
    return -1.0f;  // sensor trolling
  }

  // scaling
  float percent = (TANK_EMPTY_DISTANCE_CM - distanceCm) / (TANK_EMPTY_DISTANCE_CM - TANK_FULL_DISTANCE_CM) * 100.0f;

  // clamp. reading slightly past either calibration point shouldn't report out-of-range
  percent = std::max(0.0f, std::min(100.0f, percent));

  return percent;
}
