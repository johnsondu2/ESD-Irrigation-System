#include "system.h"

#include <WiFi.h>

#include "api.h"
#include "config.h"
#include "iot.h"
#include "tank.h"

IrrigationSystem::IrrigationSystem(Tank& tank, IoT& iot) : tank(tank), iot(iot) {}

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

  // Set output pins for the water pump and valve
  //pinMode(WATER_PUMP_PIN, OUTPUT);
  // pinMode(VALVE_PIN, OUTPUT);

  // Connect to ThingsBoard and start reporting (WiFi is already up at this point)
  iot.begin();
}

void IrrigationSystem::update() {

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

    case PUMPING: {
      // IsTankEmpty() is a cheap read of state pushed by the tank ESP32 (not a physical sensor poll), so it's
      // safe to check every loop iteration in case the tank runs dry mid-cycle
      if (IsTankEmpty()) {
        // Serial.println("Water tank ran empty mid-watering - aborting.");

        StopPump();

        // Don't reduce the deficit: we can't confirm how much water was actually delivered before running dry
        preferences.putFloat("deficit_mm", deficitMm);
        state = FAULTY;
        break;
      }

      if (millis() - wateringStart >= wateringDurationMs) {
        StopPump();

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
