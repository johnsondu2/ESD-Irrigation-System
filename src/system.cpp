#include "system.h"

#include <Preferences.h>
#include <WiFi.h>

#include "api.h"
#include "config.h"
#include "iot.h"
#include "receiver.h"

// Object to handle non-volatile storage (NVS) for persisting the soil moisture deficit across reboots
static Preferences preferences;  // NVS-backed storage so the deficit survives reboots

// global variables for the system
static State state = IDLE;                    // Initialize the system state to IDLE
static float deficitMm = 0.0f;                // Running soil moisture deficit (mm)
static float wateringTargetMm = 0.0f;         // Deficit amount being replaced by the in-progress watering event
static unsigned long wateringStart = 0;       // millis() value when the current watering event began
static unsigned long wateringDurationMs = 0;  // How long the current watering event should run for

// Tank water level state now lives in receiver.cpp (populated from the tank ESP32 over ESP-NOW) and is
// read here via IsTankEmpty()/GetTankLevelPercent()/GetTankDistanceCm() from receiver.h

// variables for weather data tracking
static unsigned long lastWeatherUpdate = 0;  // Variable to track the last time weather data was updated
static bool initialFetchDone = false;        // Flag to force the system to call the API weather data as soon as the system is booted up

void SystemBegin() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");

  // Set output pins for the water pump and valve
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);

  // Start listening for tank level readings from the tank ESP32 (WiFi is already up at this point)
  ReceiverBegin();

  // Load the persisted soil moisture deficit so it survives reboots
  preferences.begin("irrigation", false);
  deficitMm = preferences.getFloat("deficit_mm", 0.0f);

  // Connect to ThingsBoard and start reporting (WiFi is already up at this point)
  IoTSetup();
}

void SystemUpdate() {
  // Keeps the MQTT connection alive and publishes telemetry on its own timer.
  // Runs every cycle regardless of system state.
  IoTLoop();

  // Attempts to reconnect to WiFi if disconnected, but only every 10 seconds to avoid spamming the network
  if (WiFi.status() != WL_CONNECTED) {
    static unsigned long lastReconnectAttempt = 0;

    if (millis() - lastReconnectAttempt >= 10000) {
      lastReconnectAttempt = millis();
      WiFi.reconnect();
    }
  }

  switch (state) {
    case IDLE:
      // Regularly check for weather updates once a day (or on system bootup) if WiFi is connected
      if ((!initialFetchDone || (millis() - lastWeatherUpdate >= WEATHER_UPDATE_INTERVAL)) && (WiFi.status() == WL_CONNECTED)) {
        // Update the last weather update timestamp
        lastWeatherUpdate = millis();

        // Create a WeatherData struct to hold the fetched data
        WeatherData currentWeather;

        // Fetch weather data and check if successful
        if (fetchWeatherData(currentWeather)) {
          initialFetchDone = true;  // Mark that the initial weather data has been fetched from the API

          DayForecast& today = currentWeather.days[1];
          DayForecast& tomorrow = currentWeather.days[2];
          // index 1 is today's data, index 0 is yesterday's, indices 2 and 3 are the next two days' forecasts.

          // 1. Update the running water balance: ET0 is water lost, rain is water gained. Floor at 0 to avoid negative deficit.
          deficitMm += today.evapotranspiration - today.rain;
          deficitMm = max(deficitMm, 0.0f);

          // 2. Forecast-based skip guard: don't water if significant rain is expected tomorrow
          bool skipForForecast = (tomorrow.rainProbMax >= RAIN_SKIP_PROB_PERCENT) || (tomorrow.rain >= RAIN_SKIP_MM);

          // 3. Trigger watering if the deficit has crossed the threshold and rain isn't imminent
          if (deficitMm >= MAD_THRESHOLD_MM && !skipForForecast) {
            if (IsTankEmpty()) {
              // Don't open the valve/pump with no water available; persist the deficit and raise a fault instead
              // Serial.println("Water tank empty - cannot start watering.");
              preferences.putFloat("deficit_mm", deficitMm);
              state = FAULTY;
            } else {
              wateringTargetMm = deficitMm;
              wateringDurationMs = min((unsigned long)((wateringTargetMm / PUMP_FLOW_RATE_MM_PER_MIN) * 60000.0f), MAX_WATERING_MS);  // 60000 ms in a minute
              StartWater();
              wateringStart = millis();

              state = WATERING;
            }
          } else {
            // Not watering yet; persist the updated deficit so it carries over to tomorrow's fetch
            preferences.putFloat("deficit_mm", deficitMm);
          }
        } else {
          Serial.println("Failed to fetch weather. Using fallback sensor logic.");
        }
      }
      break;

    case WATERING: {
      // IsTankEmpty() is a cheap read of state pushed by the tank ESP32 (not a physical sensor poll), so it's
      // safe to check every loop iteration in case the tank runs dry mid-cycle
      if (IsTankEmpty()) {
        // Serial.println("Water tank ran empty mid-watering - aborting.");

        StopWater();

        // Don't reduce the deficit: we can't confirm how much water was actually delivered before running dry
        preferences.putFloat("deficit_mm", deficitMm);
        state = FAULTY;
        break;
      }

      if (millis() - wateringStart >= wateringDurationMs) {
        StopWater();

        // Assume the target amount was delivered; reduce the deficit and persist it
        deficitMm = max(deficitMm - wateringTargetMm, 0.0f);
        preferences.putFloat("deficit_mm", deficitMm);

        state = IDLE;
      }
      break;
    }
    case FAULTY: {
      // Check whether the tank has been refilled, and resume normal operation if so
      if (!IsTankEmpty()) {
        // Serial.println("Water tank refilled - resuming normal operation.");
        state = IDLE;
      }
      break;
    }
  }
}

void StartWater() {                    // replaced direct write with function
  digitalWrite(VALVE_PIN, HIGH);       // assuming HIGH opens the valve
  digitalWrite(WATER_PUMP_PIN, HIGH);  // assuming HIGH switches the pump on
}

void StopWater() {
  digitalWrite(VALVE_PIN, LOW);       // assuming HIGH opens the valve
  digitalWrite(WATER_PUMP_PIN, LOW);  // assuming HIGH switches the pump on
}

float GetDeficitMm() { return deficitMm; }
State GetState() { return state; }