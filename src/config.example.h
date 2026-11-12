#pragma once

/*
 * Template for src/config.h (which is gitignored and holds the real values).
 *
 * Setup: copy this file to src/config.h, then fill in your own values below.
 * Never put real credentials in this file - it is committed to the repo.
 */

// ---- WiFi credentials (secret) ----
constexpr const char* WIFI_SSID = "YOUR_WIFI_SSID";
constexpr const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ---- ThingsBoard device access token (secret) ----
// ThingsBoard Cloud -> Entities -> Devices -> (device) -> Copy access token
constexpr const char* TB_ACCESS_TOKEN = "YOUR_THINGSBOARD_ACCESS_TOKEN";

// ---- Weather API (not secret - Open-Meteo is free and keyless) ----
constexpr const char* apiUrl =
    "https://api.open-meteo.com/v1/forecast?latitude=-36.8485&longitude=174.7635"
    "&daily=rain_sum,precipitation_sum,et0_fao_evapotranspiration,precipitation_probability_max"
    "&timezone=Pacific%2FAuckland&past_days=1&forecast_days=3";
