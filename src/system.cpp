#include "system.h"

// instantiate array to store Bed objects
static Bed beds[NUM_BED_TYPES];

void SystemBegin() {
  // instantiate objects etc.
  // put objects in array
  beds[SPINACH] = Bed(SPINACH_SENSOR_PIN, SPINACH_VALVE_PINS);
  beds[LEEK] = Bed(LEEK_SENSOR_PIN, LEEK_VALVE_PINS);
  beds[SPRING_ONION] = Bed(SPRING_ONION_SENSOR_PIN, SPRING_ONION_VALVE_PINS);
  beds[GARLIC] = Bed(GARLIC_SENSOR_PIN, GARLIC_VALVE_PINS);
  beds[PARSLEY] = Bed(PARSLEY_SENSOR_PIN, PARSLEY_VALVE_PINS);
}

void SystemUpdate() {
  // call objects member function for updating state
  for (int i = 0; i < NUM_BED_TYPES; i++) {
    beds[i].Update();
  }
}
