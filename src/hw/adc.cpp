#include "hw/adc.h"

#include <ADS1115.h>

static ADS1115 adc(ADS1115_DEFAULT_ADDRESS);

void adc_hw_init() {
    // ADS1115
    Wire.begin();
    adc.initialize();
    adc.setMode(ADS1115_MODE_CONTINUOUS);
    adc.setGain(ADS1115_PGA_4P096);
    adc.setRate(ADS1115_RATE_64);
}

void adc_hw_read(AppState* s) {
    // Read measurement values
    s->rawVoltage = adc.getConversionP3GND();
    s->rawCurrent = adc.getConversionP0GND();
    // Convert to readable values
    s->voltage = static_cast<double>(s->rawVoltage) * (voltageAmplitude / adcResolution);
    s->current = static_cast<double>(s->rawCurrent) * (currentAmplitude / adcResolution);
}
