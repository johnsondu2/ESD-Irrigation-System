#include <Arduino.h>
#include "system.h"

// instantiate array to store Bed objects
static Bed beds[NUM_BEDS];

void SystemBegin() {
  // put objects in array
  beds[SPINACH] = Bed(SPINACH_SENSOR_PIN, SPINACH_VALVE_PINS);
  beds[LEEK] = Bed(LEEK_SENSOR_PIN, LEEK_VALVE_PINS);
  beds[SPRING_ONION] = Bed(SPRING_ONION_SENSOR_PIN, SPRING_ONION_VALVE_PINS);
  beds[GARLIC] = Bed(GARLIC_SENSOR_PIN, GARLIC_VALVE_PINS);
  beds[PARSLEY] = Bed(PARSLEY_SENSOR_PIN, PARSLEY_VALVE_PINS);

  // Set pin modes for all valves and sensors
  for (int i = 0; i < NUM_BEDS; i++) {
    int sensor = beds[i].GetAnalogPin(); // Get the analog pin of the Bed's sensor
    pinMode(sensor, INPUT); // Set the sensor pin to INPUT

    for (int j = 0; j < NUM_MAX_VALVES; j++) {
      int valve = beds[i].GetValvePin(j); // Get each valve pin

      if (valve != -1) { // Check if the valve pin is valid
        pinMode(valve, OUTPUT); // Set the valve pin to OUTPUT
      }
    }
  }
}

void SystemUpdate() {
  // call objects member function for updating state
  for (int i = 0; i < NUM_BEDS; i++) {
    beds[i].Update();
  }
}
