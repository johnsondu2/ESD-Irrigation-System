#pragma once

// Weather metrics for a single day
struct DayForecast {
  float rain;                // mm
  float precipitation;       // mm
  float evapotranspiration;  // mm
  int rainProbMax;           // %
};

// Holds the complete 4-day dataset
struct WeatherData {
  DayForecast days[4];  // Array to hold forecasts for 4 days
  bool valid = false;   // Flag to ensure data was successfully fetched
};

// Pass by reference so fetchWeatherData updates the struct directly
bool fetchWeatherData(WeatherData& data);
