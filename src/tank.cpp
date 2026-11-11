#include "tank.h"

#include "float.h"
#include "ultrasonic.h"
#include <Arduino.h>
#include <esp_now.h>
#include <algorithm>  // Required for std::max and std::min

Tank::Tank(FloatSwitch& lowSwitch, UltrasonicSensor& ultrasonicSensor){
        tank_float = lowSwitch;
        tank_ultrasonic = ultrasonicSensor;
        tank_float.begin();  // Initialize the float switch pin
    }

bool Tank::IsTankEmpty() {
    if (!tank_float.isClosed()) {
        return true;  // HIGH signal from the float switch indicates the tank is empty
    }
    return false;
}

float Tank::GetTankLevelPercent() {
  if (tank_ultrasonic.readDistanceCm() < 0) {
    return -1.0f;  // unknown
  }

  float percent = (TANK_EMPTY_DISTANCE_CM - tank_ultrasonic.readDistanceCm()) / (TANK_EMPTY_DISTANCE_CM - TANK_FULL_DISTANCE_CM) * 100.0f;

  // Clamp: a reading slightly past either calibration point (sensor noise, tank
  // slightly overfilled, etc.) shouldn't report an out-of-range percentage.
  percent = std::max(0.0f, std::min(100.0f, percent));

  return percent;
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
