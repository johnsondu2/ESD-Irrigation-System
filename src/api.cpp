#include "api.h"
#include "config.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>

// API url for weather data (Open-Meteo API)
static const char* apiUrl = "https://api.open-meteo.com/v1/forecast?latitude=-36.8485&longitude=174.7635&daily=rain_sum,precipitation_sum,et0_fao_evapotranspiration,precipitation_probability_max&timezone=Pacific%2FAuckland&past_days=1&forecast_days=3";

// Fetches weather data from the API and places data into the input WeatherData struct
bool fetchWeatherData(WeatherData &data) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    data.valid = false;
    return false;
  }

  HTTPClient http;

  // HTTPClient will allow a 5 second window for the GET request to complete before timing out and returning an error
  // This is important to prevent the system from hanging indefinitely if the API is unresponsive
  http.setTimeout(5000);

  // Initialize HTTP GET request
  http.begin(apiUrl);
  // Send the request and store the response code
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    // DynamicJsonDocument / JsonDocument automatically manages sizing in v7
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, http.getStream()); // http.getStream() is the JSON response from the API

    if (!error) {
      // Extract the "daily" object from the JSON response
      JsonObject daily = doc["daily"];
      
      // Populate the WeatherData struct with the fetched data
      for (int i = 0; i < 4; i++) {
        data.days[i].rain = daily["rain_sum"][i] | 0.0f;
        data.days[i].precipitation = daily["precipitation_sum"][i] | 0.0f;
        data.days[i].evapotranspiration = daily["et0_fao_evapotranspiration"][i] | 0.0f;
        data.days[i].rainProbMax = daily["precipitation_probability_max"][i] | 0;
      }

      // Mark the data as valid since it was successfully fetched and parsed
      data.valid = true;
      
      // Log Today's Data (Index 1) to ensure data is being fetched properly
      Serial.println("--- Today's Weather Data ---");
      Serial.printf("Expected Rain: %.2f mm\n", data.days[1].rain);
      Serial.printf("Expected Precipitation: %.2f mm\n", data.days[1].precipitation);
      Serial.printf("Evapotranspiration (ET0): %.2f mm\n", data.days[1].evapotranspiration);
      Serial.printf("Max Rain Prob: %d%%\n", data.days[1].rainProbMax);

      http.end(); // Clean up the HTTP connection

      return true; // Return true to indicate successful fetch
    } else {
      Serial.printf("JSON Error: %s\n", error.c_str());
    }
  } else {
    Serial.printf("HTTP Error: %d\n", httpCode);
  }

  http.end(); // Clean up the HTTP connection
  data.valid = false; // Mark data as invalid if fetch failed
  return false; // Return false to indicate failure
}
