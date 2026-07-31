#pragma once

// Enums for the system states
enum State {
    IDLE, WATERING, FAULTY
};

void SystemBegin();
void SystemUpdate();
