#include "tank.h"

#include "float_switch.h"
#include "ultrasonic.h"
#include <Arduino.h>
#include <esp_now.h>
#include <algorithm>  // Required for std::max and std::min

// References must be bound in the initializer list (they can't be assigned in the body)
Tank::Tank(FloatSwitch& floatSwitch, UltrasonicSensor& ultrasonicSensor)
    : tank_float(floatSwitch), tank_ultrasonic(ultrasonicSensor) {
  tank_float.begin();  // Initialize the float switch pin
}

bool Tank::IsTankEmpty() {
    if (!tank_float.isClosed()) {
        return true;  // HIGH signal from the float switch indicates the tank is empty
    }
    return false;
}

float Tank::GetTankLevelPercent() {
  const float distanceCm = tank_ultrasonic.readDistanceCm();
  if (distanceCm < 0) {
    return -1.0f;  // unknown
  }

  float percent = (TANK_EMPTY_DISTANCE_CM - distanceCm) / (TANK_EMPTY_DISTANCE_CM - TANK_FULL_DISTANCE_CM) * 100.0f;

  // Clamp: a reading slightly past either calibration point (sensor noise, tank
  // slightly overfilled, etc.) shouldn't report an out-of-range percentage.
  percent = std::max(0.0f, std::min(100.0f, percent));

  return percent;
}
