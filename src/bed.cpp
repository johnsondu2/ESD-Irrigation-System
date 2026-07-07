#include <Arduino.h>

#include "bed.h"
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

// Updates the state of the Bed and contains FSM logic
void Bed::Update() {
  // Update the object's sensor reading
  UpdateSensorReading();
  // Update the current millisecond count
  currentMillis_ = millis(); /* millis() is an Arduino function and will return an error for now */

  // Update state
  switch (state_) {
    case IDLE:
    /* insert FAULTY state logic here */
    // to do

    // Open valves and enter WATERING state when the moisture lower threshold is crossed
        if (sensorReading_ <= MOISTURE_LOWER_THRESHOLD) { /* LOWER_MOISTURE_THRESHOLD is set in config.h */
            // Open valves
            /* the valves logic is still up in the air. Beds will have different numbers of valves, so logic will change. loop through a valve array, maybe? */
            // to do

            // Update the time when the valves were last switched
            valvesLastSwitched_ = currentMillis_;

            /* everything between the condition and this line could be streamlined into an OpenValves() function */
            OpenValves();

            state_ = WATERING;
        }
        break;
    case WATERING:
    // Valves remain open for a set amount of time before closing, then enter WAITING state
        if (currentMillis_ - valvesLastSwitched_ >= WATERING_INTERVAL) { /* WATERING_INTERVAL is set in config.h */
            // Close valves
            // to do

            // Update the time when the valves were last switched
            valvesLastSwitched_ = currentMillis_;

            /* everything between the condition and this line could be streamlined into an CloseValves() function */
            CloseValves();

            state_ = WAITING;
        }
        break;
    case WAITING:
    // Wait for a set amount of time
    if (currentMillis_ - valvesLastSwitched_ >= WAITING_INTERVAL) { /* WAITING_INTERVAL is set in config.h */
        // Return to WATERING state if moisture readings are below the moisture upper threshold 
        if (sensorReading_ <= MOISTURE_UPPER_THRESHOLD) {
            // Open valves
            

            // Update the time when the valves were last switched
            valvesLastSwitched_ = currentMillis_;

            /* everything between the condition and this line could be replaced with the aforementioned OpenValves() function */
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

// Gets the object's sensor reading
float Bed::GetSensorReading() const {
    // to do
    return 0; /* placeholder return value */
}

// Updates the object's sensor reading
void Bed::UpdateSensorReading() {
    // Read actual sensor value
    float sensorValue = analogRead(analogPin_); /* analogRead() and map() are Arduino functions, so they will return errors for now */

    // Map the sensor value to a 0-100% range for easy readability
    sensorReading_ = map(sensorValue, SENSOR_LOWER_BOUND, SENSOR_UPPER_BOUND, 0, 100);
}

void Bed::OpenValves() {
    // to do
    for (int i = 0; i < NUM_MAX_VALVES; i++) {
        if (valves_[i] != -1) { // Check if the valve pin is valid
            // open valve logic here
            // digitalWrite(valves_[i], HIGH); // (assuming HIGH opens the valve)
        }
    }
}

void Bed::CloseValves() {
    // to do
    for (int i = 0; i < NUM_MAX_VALVES; i++) {
        if (valves_[i] != -1) { // Check if the valve pin is valid
            // close valve logic here
            //digitalWrite(valves_[i], LOW); // assuming LOW closes the valve)
        }
    }
}