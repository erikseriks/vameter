#include "hw/fan.h"

#include <Arduino.h>
#include <math.h>

#include "types/config.h"

void fan_hw_init() {
    pinMode(tempSensor1Pin, INPUT);
    pinMode(tempSensor2Pin, INPUT);
    pinMode(fanPwmPin, OUTPUT);
}

uint8_t fan_hw_calc_pwm(double temp) {
    double val = pow(round(255.0 - (temp * 255.0 / 1023.0)) / 16.0, 2.0);

    if (val < 9.0) return 0;
    if (val > 250.0) return 255;
    return static_cast<uint8_t>(val);
}

void fan_hw_apply(uint8_t pwm) {
    analogWrite(fanPwmPin, pwm);
}
