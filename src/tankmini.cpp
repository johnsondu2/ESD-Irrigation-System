#include "tankmini.h"

#include "float_switch.h"

// References must be bound in the initializer list (they can't be assigned in the body)
MiniTank::MiniTank(FloatSwitch& HighSwitch)
    : tank_float(HighSwitch) {
  tank_float.begin();  // Initialize the float switch pin
}

bool MiniTank::IsFull() {
    if (tank_float.isClosed()) {
        return true;  // LOW signal from the float switch indicates the tank is full
    }
    return false;
}
