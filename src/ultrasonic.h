#pragma once
#include <Arduino.h>

class UltrasonicSensor {
public:
    UltrasonicSensor(uint8_t echoPin, uint8_t trigPin);

    // Read the distance in cm from the sensor.
    float readDistanceCm();

private:
    float lastDistanceCm;  // last distance received
    uint8_t  _echoPin;
    uint8_t  _trigPin;
};