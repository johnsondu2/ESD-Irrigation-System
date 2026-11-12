#include <Arduino.h>

#include "config.h"
#include "float_switch.h"
#include "iot.h"
#include "system.h" 
#include "tank.h"
#include "tankmini.h"
#include "ultrasonic.h"
#include "pump.h"

// Pin configs
#define TANK_FLOAT_PIN   25   // float used by the main-tank
#define MINI_TANK_FLOAT_PIN  26   // second float, used by the mini-tank
#define ULTRA_TRIG_PIN  16   
#define ULTRA_ECHO_PIN  17
#define WATER_PUMP_PIN 5                                  // placeholder, to replace with actual pin when time comes
#define VALVE_PIN 4                                       // placeholder, to replace with actual pin when time comes
#define FLOAT_DEBOUNCE_MS  1000

FloatSwitch floatTank(TANK_FLOAT_PIN, FLOAT_DEBOUNCE_MS);
FloatSwitch floatMiniTank(MINI_TANK_FLOAT_PIN, FLOAT_DEBOUNCE_MS);
UltrasonicSensor ultrasonic(ULTRA_ECHO_PIN, ULTRA_TRIG_PIN);
Pump* pump = nullptr;  // created in setup() 
Tank* tank = nullptr;  // created in setup()
MiniTank* miniTank = nullptr;  // created in setup()
IoT* iot = nullptr;
IrrigationSystem* irrigationSystem = nullptr;

void setup() {
  floatMiniTank.begin();
  tank = new Tank(floatTank, ultrasonic);
  miniTank = new MiniTank(floatMiniTank);
  pump = new Pump(WATER_PUMP_PIN, VALVE_PIN);
  iot = new IoT(*tank);
  irrigationSystem = new IrrigationSystem(*tank, *miniTank, *iot, *pump);

  irrigationSystem->begin();
}

void loop() {
  floatTank.update();
  floatMiniTank.update();
  irrigationSystem->update();
}
