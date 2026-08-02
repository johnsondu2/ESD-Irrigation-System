#include "tank.h"

#include <Arduino.h>

// THIS IMPLEMENTATION NEEDS TO BE VALIDATED

// Triggers the single-pin ultrasonic ranger and returns the measured distance in cm,
// or -1 if no echo was received (sensor fault/out of range)
float ReadTankDistanceCm() {
  pinMode(ULTRASONIC_SENSOR_PIN, OUTPUT);
  digitalWrite(ULTRASONIC_SENSOR_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_SENSOR_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(ULTRASONIC_SENSOR_PIN, LOW);

  pinMode(ULTRASONIC_SENSOR_PIN, INPUT);
  // 30ms timeout bounds the worst-case block time (~5m range) so a failed sensor can't stall SystemUpdate()
  unsigned long duration = pulseIn(ULTRASONIC_SENSOR_PIN, HIGH, 30000UL);
  if (duration == 0) {
    return -1.0f;
  }

  return duration / 58.0f;  // standard single-pin ultrasonic ranger conversion: duration (us) / 58 = distance (cm)
}

// Returns true if the tank is empty
bool IsTankEmpty() {
  float distanceCm = ReadTankDistanceCm();
  if (distanceCm < 0) {
    return true;
  }

  return distanceCm >= TANK_EMPTY_DISTANCE_CM;
}

float GetTankLevelPercent() {
  float distanceCm = ReadTankDistanceCm();
  if (distanceCm < 0 || distanceCm > (TANK_EMPTY_DISTANCE_CM + 50.0f)) {
    return -1.0f;  // sensor trolling
  }

  float percent = (TANK_EMPTY_DISTANCE_CM - distanceCm) / (TANK_EMPTY_DISTANCE_CM - TANK_FULL_DISTANCE_CM) * 100.0f;

  // Clamp: a reading slightly past either calibration point (sensor noise, tank
  // slightly overfilled, etc.) shouldn't report an out-of-range percentage.
  percent = std::max(0.0f, std::min(100.0f, percent));

  return percent;
}
