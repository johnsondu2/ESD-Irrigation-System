#pragma once

#include "float_switch.h"
#include "ultrasonic.h"

#define TANK_EMPTY_DISTANCE_CM 116.0f  // placeholder: distance (cm) from sensor to water surface that counts as "empty"; needs calibration against actual tank geometry
#define TANK_FULL_DISTANCE_CM 5.0f     // guess

// write tank as a class, with the below functions, and a float class member, and a ultrasonic class member
class Tank {
public:
  Tank(FloatSwitch& lowSwitch, UltrasonicSensor& ultrasonicSensor);

  float GetTankLevelPercent();
  bool IsTankEmpty();

private:
    FloatSwitch& tank_float;
    UltrasonicSensor& tank_ultrasonic;

};
