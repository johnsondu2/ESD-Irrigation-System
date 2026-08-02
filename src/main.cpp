#include <Arduino.h>

#include "config.h"
#include "system.h"

void setup() {
  Serial.begin(115200);
  SystemBegin();
}

void loop() {
  SystemUpdate();
}
