#pragma once

/* 
// below code is for the customised set time, not yet implemented
// NTP server used to sync the system clock once WiFi is up
#define WEATHER_CHECK_HOUR 15                             // Local hour (24h) at which to check for weather updates once daily
#define NZ_TZ "NZST-12NZDT,M9.5.0,M4.1.0/3"                // POSIX TZ string for New Zealand; handles NZST/NZDT DST transitions automatically
#define NTP_SERVER "pool.ntp.org"     
*/

#define WEATHER_UPDATE_INTERVAL 86400000UL  // 24 hours in milliseconds

#define WATER_PUMP_PIN 5                                  // Example pin for water pump (to do: replace with actual pin number)
#define VALVE_PIN 4                                       // Example pin for valve (to do: replace with actual pin number)

// constants for the control logic decision-making
// NOTE: all of these are placeholders and need real-world testing/calibration
#define MAD_THRESHOLD_MM 20.0f                                                          // deficit (mm) that triggers watering (Management Allowable Depletion)
#define RAIN_SKIP_PROB_PERCENT 70                                                       // skip watering if tomorrow's rain probability >= this
#define RAIN_SKIP_MM 5.0f                                                               // skip watering if tomorrow's forecasted rain >= this
#define PUMP_FLOW_RATE_L_PER_MIN 5.0f                                                   // placeholder: litres/minute delivered by the pump
#define TOTAL_IRRIGATED_AREA_M2 10.0f                                                   // placeholder: combined bed area in square metres
#define PUMP_FLOW_RATE_MM_PER_MIN (PUMP_FLOW_RATE_L_PER_MIN / TOTAL_IRRIGATED_AREA_M2)  // 1L over 1m^2 = 1mm depth
#define MAX_WATERING_MS 1800000UL                                                       // 30 minute safety cap on a single watering event
//#define TANK_CHECK_INTERVAL_MS 1000UL                                                   // how often to poll the tank sensor while watering
//#define FAULT_RECHECK_INTERVAL_MS 1800000UL                                             // how often to check whether the tank has been refilled while FAULTY

// Enums for the system states
enum State {
  IDLE,
  WATERING,
  FAULTY
};

void SystemBegin();
void SystemUpdate();

void StartWater();
void StopWater();

float GetDeficitMm();
State GetState();

// new stuff
/*
void CheckTime();
bool ProcessWeatherData();
*/