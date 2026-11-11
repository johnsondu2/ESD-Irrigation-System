#include "ultrasonic.h"

#include <Arduino.h>

// Triggers the ultrasonic ranger and returns the measured distance in cm,
// or -1.0f if no echo was received (sensor fault/out of range)
float UltrasonicSensor::readDistanceCm() {

  // Ensure trigger pin is low
  digitalWrite(_trigPin, LOW);
  delayMicroseconds(2);

  // 10 us pulse to the trigger pin
  digitalWrite(_trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(_trigPin, LOW);

  // Read echo pulse duration with a 30,000us max timeout as a fail-safe
  long duration = pulseIn(_echoPin, HIGH, 30000);

  // guard
  if (duration == 0) {
    Serial.println("ultra: Error / Timeout");
    return -1.0f;
  }

  // calc distance, using speed of air = 343 m/s, and dividing by 2 for the round trip
  float dist = (duration * 0.0343f) / 2.0f;

  Serial.print("ultra: ");
  Serial.print(dist);
  Serial.println(" cm");

  return dist;
}
