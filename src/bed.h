#pragma once
#include <config.h>

class Bed {
 private:
  int analogPin_; // analog pin unique to the Bed's sensor and declared in config.h (e.g. #define KALE_SENSOR A0)
  float sensorReading_; // sensor reading
  State state_; // current Bed state
  unsigned long valvesLastSwitched_; // millis() value when the valves were last switched
  unsigned long currentMillis_;

 public:
  Bed(int analogPin);
  ~Bed();

  void Update();
  void UpdateSensorReading();

  float GetSensorReading() const;
};
