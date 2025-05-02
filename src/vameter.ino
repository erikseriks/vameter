#include <uptime_formatter.h>
#include <uptime.h>
#include <U8glib.h>
#include <PString.h>
#include <ADS1115.h>

U8GLIB_SSD1309_128X64 u8g(4, 5, 8, 7, 6); // SPI Com: SCK/SCL, MOSI/SDA, CS, DC, RES
ADS1115 adc0(ADS1115_DEFAULT_ADDRESS);

constexpr int lineMaxLength = 78; // Underline indicator for VA

constexpr double voltageAmplitude = 40.0;
constexpr double currentAmplitude = 50.0;
constexpr double analogReadRes = 32767.0;
int16_t voltageSensorValue;
int16_t currentSensorValue;
double voltage = 0.000;
double current = 0.000;
constexpr int filterWeight = 0.9;

constexpr int tempSensor1Pin = 14;
constexpr int tempSensor2Pin = 15;
constexpr int fanPwmPin = 3;
double temp1 = 1023.0;
double temp2 = 1023.0;
double fanPwmValue = 255;
unsigned long fanControlTime = 0;
double highestTemp = 1023.0;
double avgTemperature = 1023.0;

double getVoltage() {
    // Read measurement values
    voltageSensorValue = adc0.getConversionP3GND();
    // Convert to readable values
    voltage = voltageSensorValue * (voltageAmplitude / analogReadRes);

    return voltage;
}

double getCurrent() {
    // Read measurement values
    currentSensorValue = adc0.getConversionP0GND();
    // Convert to readable values
    current = currentSensorValue * (currentAmplitude / analogReadRes);

    return current;
}

double getHighestVal(const double a, const double b) {
    if (a > b) {
        return a;
    }

    return b;
}

void updateAvgTemp() {
    avgTemperature = floor(((avgTemperature + highestTemp) / 2) / 10) * 10;
}

double calculateFanPwm(const double measurement) {
    // Calculate exponential pwm value from temperature measurement and precalculated values
    fanPwmValue = pow((round(255 - (measurement * 255 / 1023.0)) / 16), 2);

    if (fanPwmValue < 9) {
        fanPwmValue = 0;
    }

    if (fanPwmValue > 250) {
        fanPwmValue = 255;
    }

    return fanPwmValue;
}

void draw() {
    const double watts = voltage * current;

    char voltageString[16];
    char currentString[16];
    char wattsString[16];

    const int lineWidth1 = voltage >= 0 ? voltageSensorValue / (analogReadRes / lineMaxLength) : 0;
    const int lineWidth2 = current >= 0 ? currentSensorValue / (analogReadRes / lineMaxLength) : 0;

    // Voltage
    PString PVoltage(voltageString, sizeof(voltageString));
    if (abs(voltage) < 1) {
        PVoltage.print(round(voltage * 1000));
        PVoltage.print(" mV");
    } else {
        PVoltage.print(voltage);
        PVoltage.print(" V");
    }

    // Current
    PString PCurrent(currentString, sizeof(currentString));
    if (abs(current) < 1) {
        PCurrent.print(round(current * 1000));
        PCurrent.print(" mA");
    } else {
        PCurrent.print(current);
        PCurrent.print(" A");
    }

    // Watts
    PString PWatts(wattsString, sizeof(wattsString));
    constexpr double epsilon = 0.000001;
    if (abs(watts) < 0.001 - epsilon) {
        PWatts.print(round(watts * 1000000));
        PWatts.print("uW");
    } else if (abs(watts) < 1.0 - epsilon) {
        PWatts.print(round(watts * 1000));
        PWatts.print("mW");
    } else if (abs(watts) > 1000.0 + epsilon) {
        PWatts.print(watts / 1000);
        PWatts.print("kW");
    } else {
        PWatts.print(watts);
        PWatts.print("W");
    }

    constexpr int underlineWidth = lineMaxLength + 2;

    // Draw strings and scale line
    u8g.setFont(u8g_font_timR18);

    u8g.drawStr(0, 20, voltageString);
    u8g.drawBox(0, 25, lineWidth1, 2);
    u8g.drawLine(underlineWidth, 25, underlineWidth, 29);
    u8g.drawLine(0, 29, underlineWidth, 29);

    u8g.drawStr(0, 54, currentString);
    u8g.drawBox(0, 59, lineWidth2, 2);
    u8g.drawLine(underlineWidth, 59, underlineWidth, 63);
    u8g.drawLine(0, 63, underlineWidth, 63);

    // Set font for watts
    u8g.setFont(u8g_font_helvR08);
    // Draw string for watts
    u8g.drawStr(86, 64, wattsString);

    // Uptime
    char d[8];
    char h[8];
    char m[8];
    char s[8];
    int textLineYPosition = 21;

    u8g.drawStr(90, 8, "Uptime");

    // Set font for uptime counter
    u8g.setFont(u8g_font_profont12);

    if (uptime::getDays()) {
        PString upTimeStringD(d, sizeof(d));
        upTimeStringD.print(uptime::getDays());
        upTimeStringD.print("d");
        u8g.drawStr(92, textLineYPosition, upTimeStringD);
        textLineYPosition += 10;
    }
    if (uptime::getHours()) {
        PString upTimeStringH(h, sizeof(h));
        upTimeStringH.print(uptime::getHours());
        upTimeStringH.print(" h");
        u8g.drawStr(92, textLineYPosition, upTimeStringH);
        textLineYPosition += 10;
    }
    if (uptime::getMinutes()) {
        PString upTimeStringM(m, sizeof(m));
        upTimeStringM.print(uptime::getMinutes());
        upTimeStringM.print(" min");
        u8g.drawStr(92, textLineYPosition, upTimeStringM);
        textLineYPosition += 10;
    }
    if (uptime::getSeconds()) {
        PString upTimeStringS(s, sizeof(s));
        upTimeStringS.print(uptime::getSeconds());
        upTimeStringS.print(" s");
        u8g.drawStr(92, textLineYPosition, upTimeStringS);
    }
}

void setup() {
    // Set frequency of D3 and D11
    TCCR2B = (TCCR2B & B11111000) | B00000001; // set timer 2 divisor to 1 for PWM frequency of 31372.55 Hz

    // Set analog read reference to internal 1.1 V
    analogReference(INTERNAL);

    // Display
    u8g.sleepOn();
    u8g.setColorIndex(1);

    // ADS1115
    Wire.begin();
    adc0.initialize();
    adc0.setMode(ADS1115_MODE_CONTINUOUS);
    adc0.setGain(ADS1115_PGA_4P096);
    adc0.setRate(ADS1115_RATE_64);

    // Fan control
    pinMode(tempSensor1Pin, INPUT);
    pinMode(tempSensor2Pin, INPUT);
    pinMode(fanPwmPin, OUTPUT);
}

void loop() {
    // Set VA values
    voltage = getVoltage();
    current = getCurrent();

    // Read temperatures
    temp1 = analogRead(tempSensor1Pin);
    temp2 = analogRead(tempSensor2Pin);
    highestTemp = getHighestVal(temp1, temp2);

    // Update average temperature
    updateAvgTemp();

    // Set fan speed
    if (fanControlTime < millis()) {
        analogWrite(fanPwmPin, calculateFanPwm(avgTemperature));
        fanControlTime = millis() + 1000;
    }

    // Calculate uptime
    uptime::calculateUptime();

    // Draw a picture loop by 8bit lines
    u8g.firstPage();
    do {
        draw();
    } while (u8g.nextPage());
}
