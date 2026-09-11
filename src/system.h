#pragma once
 
// NOTE: this value can be changed to a smaller value for testing purposes, but should be set to 3600000UL (1 hour) afterwards
#define TANK_CHECK_INTERVAL_MS 3600000UL // poll the ultrasonic sensor every 1 hour to check the water level in the tank
 
// Function declarations
void SystemBegin();
void SystemUpdate();
void usonicsetup(void);
long usonic(long utimeout);
 