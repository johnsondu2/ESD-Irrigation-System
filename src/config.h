#pragma once

// to do: hardware pins

// sensors with placeholder pin values
const int SPINACH_SENSOR_PIN = 1;
const int LEEK_SENSOR_PIN = 2;
const int SPRING_ONION_SENSOR_PIN = 3;
const int GARLIC_SENSOR_PIN = 4;
const int PARSLEY_SENSOR_PIN = 5;

// valves with placeholder pin values
#define NUM_MAX_VALVES 10
const int SPINACH_VALVE_PINS[NUM_MAX_VALVES] = {2, 3, 4, -1, -1, -1, -1, -1, -1, -1}; // only 3 valves for spinach
const int LEEK_VALVE_PINS[NUM_MAX_VALVES] = {5, 6, 7, -1, -1, -1, -1, -1, -1, -1};
const int SPRING_ONION_VALVE_PINS[NUM_MAX_VALVES] = {8, 9, 10, -1, -1, -1, -1, -1, -1, -1};
const int GARLIC_VALVE_PINS[NUM_MAX_VALVES] = {11, 12, 13, -1, -1, -1, -1, -1, -1, -1};
const int PARSLEY_VALVE_PINS[NUM_MAX_VALVES] = {14, 15, 16, -1, -1, -1, -1, -1, -1, -1};

// Enum for FSM Bed states
enum State {
    IDLE, WATERING, WAITING, FAULTY
};

// Enum for Bed types, used specifying bed array size and indexing into the array
// NUM_BED_TYPES must stay last so its value always equals the number of real bed types above it
enum BedType {
    SPINACH, LEEK, SPRING_ONION, GARLIC, PARSLEY, NUM_BED_TYPES
};

// Bed class variables
/* NOTE: none of these values are official and they are all subject to testing */
const int MOISTURE_LOWER_THRESHOLD = 30; /* 30%; needs to be tested */
const int MOISTURE_UPPER_THRESHOLD = 70; /* 70%; needs to be tested */
const unsigned long WATERING_INTERVAL = 1200000; /* 20 minutes; needs to be tested */
const unsigned long WAITING_INTERVAL = 1200000; /* 20 minutes; needs to be tested */
const int SENSOR_LOWER_BOUND = 0; /* this value will depend on the sensor chosen */
const int SENSOR_UPPER_BOUND = 4096; /* this value will depend on the sensor chosen */
