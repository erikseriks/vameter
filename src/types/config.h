#pragma once

#include <stdint.h>

static constexpr int16_t lineMaxLength = 78;

static constexpr double voltageAmplitude = 40.0;
static constexpr double currentAmplitude = 50.0;
static constexpr double adcResolution = 32767.0;

static constexpr double epsilon = 1e-6;

static constexpr int16_t wattsHighThreshold = 1000;
static constexpr int16_t wattsLowThreshold = 1;

static constexpr uint8_t tempSensor1Pin = 14;
static constexpr uint8_t tempSensor2Pin = 15;
static constexpr uint8_t fanPwmPin = 3;

static constexpr uint16_t fanControlIntervalMs = 1000;
