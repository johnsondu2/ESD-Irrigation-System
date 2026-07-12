#pragma once
#include "bed.h"
#include "config.h"

void SystemBegin();
void SystemUpdate();

Bed* GetBeds();  // returns pointer to beds

// to do