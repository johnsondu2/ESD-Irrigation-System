#include "bed.h"

#include <Arduino.h>

#include "config.h"

Bed::Bed(const int analogPin, const int valves[NUM_MAX_VALVES]) {
  // Sets unique analog pin for the Bed's sensor
  analogPin_ = analogPin;

  // Sets unique digital pins for the Bed's valves
  // using length of valves array
  for (int i = 0; i < NUM_MAX_VALVES; i++) {
    valves_[i] = valves[i];
  }

  // All Beds start in the IDLE state
  state_ = IDLE;

  // Time when valves were last switched is set to 0
  valvesLastSwitched_ = 0;
}

Bed::~Bed() {
}

Bed::Bed() {  // default constructor needed for initialising blank array of beds
}

// Updates the state of the Bed and contains FSM logic
void Bed::Update() {
  // Update the object's sensor reading
  UpdateSensorReading();
  // Update the current millisecond count
  currentMillis_ = millis();

  // Update state
  switch (state_) {
    case IDLE:
      /* insert FAULTY state logic here */
      // to do

      // Open valves and enter WATERING state when the moisture lower threshold is crossed
      if (sensorReading_ <= MOISTURE_LOWER_THRESHOLD) {
        // Open valves and update the time when the valves were last switched
        OpenValves();

        state_ = WATERING;
      }
      break;
    case WATERING:
      // Valves remain open for a set amount of time before closing, then enter WAITING state
      if (currentMillis_ - valvesLastSwitched_ >= WATERING_INTERVAL) {
        // Close valves and update the time when the valves were last switched
        CloseValves();

        state_ = WAITING;
      }
      break;
    case WAITING:
      // Wait for a set amount of time
      if (currentMillis_ - valvesLastSwitched_ >= WAITING_INTERVAL) {
        // Return to WATERING state if moisture readings are below the moisture upper threshold
        if (sensorReading_ <= MOISTURE_UPPER_THRESHOLD) {
          // Open valves and update the time when the valves were last switched
          OpenValves();

          state_ = WATERING;
        } else {
          // Enter IDLE state if moisture readings are above the moisture upper threshold
          state_ = IDLE;
        }
      }
      break;
    case FAULTY:
      // to do
      break;
  }
}

// Updates the object's sensor reading
void Bed::UpdateSensorReading() {
  // Read actual sensor value
  float sensorValue = analogRead(analogPin_);

  // Map the sensor value to a 0-100% range for easy readability
  sensorReading_ = map(sensorValue, SENSOR_LOWER_BOUND, SENSOR_UPPER_BOUND, 0, 100);
}

// Returns the Bed's analog pin
int Bed::GetAnalogPin() {
  return analogPin_;
}

// Returns a single valve pin
int Bed::GetValvePin(int position) {
  return valves_[position];
}

// Opens the Bed's valves and updates the time when the valves were last switched
void Bed::OpenValves() {
  // to do
  for (int i = 0; i < NUM_MAX_VALVES; i++) {
    if (valves_[i] != -1) {  // Check if the valve pin is valid
      // Open the valve
      digitalWrite(valves_[i], HIGH);  // (assuming HIGH opens the valve)
    }
  }

  // Update the time when the valves were last switched
  valvesLastSwitched_ = currentMillis_;
}

// Closes the Bed's valves and updates the time when the valves were last switched
void Bed::CloseValves() {
  // to do
  for (int i = 0; i < NUM_MAX_VALVES; i++) {
    if (valves_[i] != -1) {  // Check if the valve pin is valid
      // Close the valve
      digitalWrite(valves_[i], LOW);  // assuming LOW closes the valve
    }
  }

  // Update the time when the valves were last switched
  valvesLastSwitched_ = currentMillis_;
}

// Returns bed's sensor reading
float Bed::GetSensorReading() {
  return sensorReading_;
}