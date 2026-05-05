#pragma once

#include <stdint.h>

typedef struct {
    double voltage;
    double current;
    double watts;
    double highestTemp;
    double avgTemp;
    int16_t rawVoltage;
    int16_t rawCurrent;
} AppState;
