#pragma once
#include <config.h>

class Bed {
 private:
  int analogPin_; // analog pin unique to the Bed's sensor and declared in config.h (e.g. #define KALE_SENSOR A0)
  int valves_[10]; // array of digital pins unique to the Bed's valves and declared in config.h (e.g. #define KALE_VALVES {2, 3, 4})
  // initialised with 10 max valves, any unused valves will be set to -1 in config.h (e.g. #define KALE_VALVES {2, 3, 4, -1, -1, -1, -1, -1, -1, -1})

  float sensorReading_; // sensor reading
  State state_; // current Bed state
  unsigned long valvesLastSwitched_; // millis() value when the valves were last switched
  unsigned long currentMillis_;

 public:
  Bed(int analogPin, int valves[10]);
  ~Bed();

  void Update();
  void UpdateSensorReading();

  float GetSensorReading() const;

  void OpenValves();
  void CloseValves();
};
