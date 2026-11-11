#pragma once

// Tank geometry calibration (same sensor/tank as before; now read by a separate ESP32 and sent over ESP-NOW)
#define TANK_EMPTY_DISTANCE_CM 116.0f  // placeholder: distance (cm) from sensor to water surface that counts as "empty"; needs calibration against actual tank geometry
#define TANK_FULL_DISTANCE_CM 5.0f     // guess
// the above might not be necessary?

// write tank as a class, with the below functions, and a float class member, and a ultrasonic class member
class Tank {
public:
  Tank(FloatSwitch& lowSwitch, UltrasonicSensor& ultrasonicSensor);

  // Tank fill level as a percentage, derived from the last received distance (-1 if unknown/stale/fault)
  float GetTankLevelPercent();
  bool IsTankEmpty();

private:
    FloatSwitch& tank_float;
    UltrasonicSensor& tank_ultrasonic;

};
