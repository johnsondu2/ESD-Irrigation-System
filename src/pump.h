#pragma once
#include <Arduino.h>

class Pump {
public:
    Pump(uint8_t pump_pin_, uint8_t valve_pin_);

    // Turn the pump on.
    void turnOn();

    // Turn the pump off.
    void turnOff();

    bool isOn();

private:
    uint8_t pump_pin;
    uint8_t valve_pin;
};
