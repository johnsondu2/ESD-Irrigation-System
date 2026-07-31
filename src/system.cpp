#include "system.h"
#include "config.h"
#include "api.h"

#include <WiFi.h>

#define WEATHER_UPDATE_INTERVAL 86400000UL // 24 hours in milliseconds

static unsigned long lastWeatherUpdate = 0; // Variable to track the last time weather data was updated
static bool initialFetchDone = false; // Flag to force the system to call the API weather data as soon as the system is booted up
static State state = IDLE; // Initialize the system state to IDLE

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
}

void SystemUpdate() {
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
          initialFetchDone = true; // Mark that the initial weather data has been fetched from the API

          ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          // INSERT DECISION MAKIONG LOGIC HERE

          // examples of how to fetch weather data for each data type; you can use these to call the data for implementing decision logic
          float todaysRain = currentWeather.days[1].rain;
          float todaysPrecipitation = currentWeather.days[1].precipitation;
          float todaysET0 = currentWeather.days[1].evapotranspiration;
          int todaysRainProbMax = currentWeather.days[1].rainProbMax;
          // the index 1 is used to get today's data. index 0 is yesterday's data, and indices 2 and 3 are for the next two days' forecasts.

          ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

          // to do: implement decision making logic, open valve, switch on pump, transition to watering state, etc
        } else {
          Serial.println("Failed to fetch weather. Using fallback sensor logic.");
        }
      }
      break;
    case WATERING:
      // to do
        break;
    case FAULTY:
        // to do
        break;
    }
}
