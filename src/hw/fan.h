#pragma once

#include <Arduino.h>

void fan_hw_init();
uint8_t fan_hw_calc_pwm(double temp);
void fan_hw_apply(uint8_t pwm);
