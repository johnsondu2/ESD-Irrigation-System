#pragma once

// to do: hardware pins

// Enum for FSM Bed states
enum State {
    IDLE, WATERING, WAITING, FAULTY
};

// Bed class variables
/* NOTE: none of these values are official and they are all subject to testing */
const int MOISTURE_LOWER_THRESHOLD = 30; /* 30%; needs to be tested */
const int MOISTURE_UPPER_THRESHOLD = 70; /* 70%; needs to be tested */
const unsigned long WATERING_INTERVAL = 1200000; /* 20 minutes; needs to be tested */
const unsigned long WAITING_INTERVAL = 1200000; /* 20 minutes; needs to be tested */
const int SENSOR_LOWER_BOUND = 0; /* this value will depend on the sensor chosen */
const int SENSOR_UPPER_BOUND = 4096; /* this value will depend on the sensor chosen */
