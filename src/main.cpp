#include <Arduino.h>

#include "config.h"
#include "float_switch.h"
#include "iot.h"
#include "system.h" 
#include "tank.h"
#include "ultrasonic.h"

// Pin configs
#define TANK_FLOAT_PIN   25   // float used by the main-tank
#define MINI_TANK_FLOAT_PIN  26   // second float, used by the mini-tank
#define ULTRA_TRIG_PIN  16   
#define ULTRA_ECHO_PIN  17
#define FLOAT_DEBOUNCE_MS  1000

FloatSwitch floatTank(TANK_FLOAT_PIN, FLOAT_DEBOUNCE_MS);
FloatSwitch floatMiniTank(MINI_TANK_FLOAT_PIN, FLOAT_DEBOUNCE_MS);
UltrasonicSensor ultrasonic(ULTRA_ECHO_PIN, ULTRA_TRIG_PIN);
Tank* tank = nullptr;  // created in setup() so pin setup happens after the Arduino core is up
IoT* iot = nullptr;
IrrigationSystem* irrigationSystem = nullptr;

void setup() {
  floatMiniTank.begin();
  tank = new Tank(floatTank, ultrasonic);
  iot = new IoT(*tank);
  irrigationSystem = new IrrigationSystem(*tank, *iot);

  irrigationSystem->begin();
}

void loop() {
  floatTank.update();
  floatMiniTank.update();

  iot->loop(irrigationSystem->GetState(), irrigationSystem->GetDeficitMm());
  irrigationSystem->update();
}
