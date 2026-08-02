#pragma once

#define ULTRA_ECHO 12  // Example pin for ultrasonic sensor (to do: replace with actual pin number)
#define ULTRA_TRIG 14

#define TANK_EMPTY_DISTANCE_CM 180.0f  // placeholder: 116 distance (cm) from sensor to water surface that counts as "empty"; needs calibration against actual tank geometry

#define TANK_FULL_DISTANCE_CM 5.0f  // guess

float ReadTankDistanceCm();
bool IsTankEmpty();

float GetTankLevelPercent();
