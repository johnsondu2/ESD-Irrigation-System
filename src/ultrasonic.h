#pragma once
#include <Arduino.h>

class UltrasonicSensor {
public:
    UltrasonicSensor(uint8_t echoPin_, uint8_t trigPin_);

    // Read the distance in cm from the sensor.
    float readDistanceCm();

private:
    float lastDistanceCm;  // last distance received
    uint8_t  echoPin;
    uint8_t  trigPin;
};