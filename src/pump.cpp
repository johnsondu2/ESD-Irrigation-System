#include "pump.h"

#include <Arduino.h>

Pump::Pump(uint8_t pump_pin_, uint8_t valve_pin_) : pump_pin(pump_pin_), valve_pin(valve_pin_) {
    //pinMode(pump_pin, OUTPUT);
    //pinMode(valve_pin, OUTPUT);

}

void Pump::turnOn() {
    //digitalWrite(pump_pin, HIGH);
}

void Pump::turnOff() {
    //digitalWrite(pump_pin, LOW);
}

bool Pump::isOn() {
    //return digitalRead(pump_pin) == HIGH;
    return false;
}
