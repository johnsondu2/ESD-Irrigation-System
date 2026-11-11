// Sensor bench test: float switches, ultrasonic sensor and Tank class.
// Build/upload with:   pio run -e sensor_test -t upload && pio device monitor -e sensor_test
// (the sensor_test env in platformio.ini only compiles this file + the sensor classes)

#include <Arduino.h>

#include "float_switch.h"
#include "tank.h"
#include "ultrasonic.h"

// Pin configs
#define TANK_FLOAT_PIN   25   // float used by the main-tank
#define MINI_TANK_FLOAT_PIN  26   // second float, used by the mini-tank
#define ULTRA_TRIG_PIN  16   
#define ULTRA_ECHO_PIN  17   

#define FLOAT_DEBOUNCE_MS  50
#define PRINT_INTERVAL_MS  1000UL

FloatSwitch floatLow(TANK_FLOAT_PIN, FLOAT_DEBOUNCE_MS);
FloatSwitch floatHigh(MINI_TANK_FLOAT_PIN, FLOAT_DEBOUNCE_MS);
UltrasonicSensor ultrasonic(ULTRA_ECHO_PIN, ULTRA_TRIG_PIN);
Tank* tank = nullptr;  // created in setup() so pin setup happens after the Arduino core is up