#include "system.h"

#include <WiFi.h>

#include "api.h"
#include "config.h"
#include "iot.h"
#include "tank.h"
#include "tankmini.h"
#include "pump.h"

#define BTN_PIN 0

IrrigationSystem::IrrigationSystem(Tank& tank, MiniTank& miniTank, IoT& iot, Pump& pump) : tank(tank), miniTank(miniTank), iot(iot), pump(pump) {}

void IrrigationSystem::begin() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  delay(1000);  // give the serial monitor time to connect

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

   Serial.println("\nConnected to WiFi!");

  // Load the persisted soil moisture deficit so it survives reboots
  preferences.begin("irrigation", false);
  deficitMm = preferences.getFloat("deficit_mm", 0.0f);

  // Connect to ThingsBoard and start reporting (WiFi is already up at this point)
  iot.begin();

  pinMode(BTN_PIN, INPUT_PULLUP);
}

void IrrigationSystem::update() {

  // Keeps the MQTT connection alive and publishes telemetry on its own timer.
    // Runs every cycle regardless of system state.
    iot.loop(state, deficitMm);

  // Attempts to reconnect to WiFi if disconnected, but only every 10 seconds to avoid spamming the network
  if (WiFi.status() != WL_CONNECTED) {
    static unsigned long lastReconnectAttempt = 0;

    if (millis() - lastReconnectAttempt >= 10000) {
      lastReconnectAttempt = millis();
      WiFi.reconnect();
    }
  }

  // print state every second for debugging
    static unsigned long lastStatePrint = 0;
    if (millis() - lastStatePrint >= 1000) {
      lastStatePrint = millis();
      Serial.printf("[%lu ms] Current state: %d, deficit: %.2f mm\n", millis(), state, deficitMm);
    }

  switch (state) {

    case IDLE:

      if (tank.IsTankEmpty()) {
        Serial.println("Water tank is empty - cannot water the mini tank.");
        state = NO_WATER;
        break;
      } 

      if (digitalRead(BTN_PIN) == LOW) {   // pressed pulls the pin to GND
       Serial.println("BOOT pressed");
       delay(300);                        // crude debounce for a test
       state = PUMPING;
       needToPump = true;  // set the flag to indicate that watering is needed
       break;
      }
      break;
     

    case PUMPING: {
      // IsTankEmpty() is a cheap read of state pushed by the tank ESP32 (not a physical sensor poll), so it's
      // safe to check every loop iteration in case the tank runs dry mid-cycle
      if (miniTank.IsFull()) {
        Serial.println("Mini tank is now full");

        StopPump();
        state = IDLE;
        needToPump = false;  // reset the flag since watering is complete
        break;
      }

      if (tank.IsTankEmpty()) {
        Serial.println("Water tank ran empty mid-watering - aborting.");

        StopPump();
        state = NO_WATER;
        break;
      }

      break;
    }
    case NO_WATER: {
      // Check whether the tank has been refilled, and resume normal operation if so
      if (!tank.IsTankEmpty()) {
        if (needToPump) {
          StartPump();  // resume watering if it was interrupted
          Serial.println("Mini tank refilled - resuming normal operation.");
          state = PUMPING;
        } else {
          Serial.println("Water tank refilled - returning to IDLE state.");
          state = IDLE;
        }
        break;
      }
      break;
    }

    case FAULTY: {
      // none here for now, will add logic to handle faults in the future
      break;
    }
  }
}

void IrrigationSystem::StartPump() {                   
  Serial.println("Starting pump and opening valve");
  //digitalWrite(VALVE_PIN, HIGH);       // assuming HIGH opens the valve
  //digitalWrite(WATER_PUMP_PIN, HIGH);  // assuming HIGH switches the pump on
}

void IrrigationSystem::StopPump() {
  Serial.println("Stopping pump and closing valve");
  // digitalWrite(VALVE_PIN, LOW);       // assuming HIGH opens the valve
  // digitalWrite(WATER_PUMP_PIN, LOW);  // assuming HIGH switches the pump on
}

float IrrigationSystem::GetDeficitMm() { return deficitMm; }
State IrrigationSystem::GetState() { return state; }
