#pragma once

#include "float_switch.h"

// write tank as a class, with the below functions, and a float class member
class MiniTank {
public:
  MiniTank(FloatSwitch& HighSwitch);

  bool IsPumpedWaterEnough();

private:
    FloatSwitch& tank_float;

};
