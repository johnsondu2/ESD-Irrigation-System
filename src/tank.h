#pragma once

#define ULTRASONIC_SENSOR_PIN 15 // Example pin for ultrasonic sensor (to do: replace with actual pin number)
#define TANK_EMPTY_DISTANCE_CM 40.0f // placeholder: distance (cm) from sensor to water surface that counts as "empty"; needs calibration against actual tank geometry

float ReadTankDistanceCm();
bool IsTankEmpty();
