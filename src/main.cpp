#include <Arduino.h>

#include "config.h"
#include "system.h"

void setup() {
  SystemBegin();
}

void loop() {
  SystemUpdate();
}
