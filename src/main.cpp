#include <Arduino.h>
#include <math.h>
#include <uptime.h>

#include "hw/adc.h"
#include "hw/display.h"
#include "hw/fan.h"
#include "types/state.h"

void setup() {
    // Set timer 2 divisor to 1 for PWM frequency of 31372.55 Hz
    TCCR2B = (TCCR2B & B11111000) | B00000001;

    // Set analog read reference to internal 1.1 V
    analogReference(INTERNAL);

    adc_hw_init();
    fan_hw_init();
    display_hw_init();
}

void loop() {
    // Set VA values
    static AppState state{0, 0, 0, 1023.0, 1023.0, 0, 0};
    adc_hw_read(&state);

    // Read temperatures
    const int16_t t1 = analogRead(tempSensor1Pin);
    const int16_t t2 = analogRead(tempSensor2Pin);
    state.highestTemp = t1 > t2 ? static_cast<double>(t1) : static_cast<double>(t2);

    // Update average temperature
    state.avgTemp = floor(((state.avgTemp + state.highestTemp) / 2.0) / 10.0) * 10.0;

    // Set fan speed
    static unsigned long lastFanMs = 0;
    const unsigned long now = millis();
    if (now >= lastFanMs) {
        fan_hw_apply(fan_hw_calc_pwm(state.avgTemp));
        lastFanMs = now + fanControlIntervalMs;
    }

    // Calculate watts
    state.watts = state.voltage * state.current;

    // Calculate uptime
    uptime::calculateUptime();

    // Draw to display
    display_hw_clear();
    display_hw_render(&state);
    display_hw_send();
}
