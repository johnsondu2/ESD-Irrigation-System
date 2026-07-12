#include <Arduino.h>

#include "config.h"
#include "iot.h"
#include "system.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  SystemBegin();

  IoTSetup();

  IoTSetBeds(GetBeds(), NUM_BEDS);
}

void loop() {
  SystemUpdate();
  IoTLoop();
  IoTSendTelemetry(GetBeds(), NUM_BEDS);
}
