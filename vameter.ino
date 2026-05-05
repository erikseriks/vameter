#include <uptime_formatter.h>
#include <uptime.h>
#include <U8g2lib.h>
#include <PString.h>
#include <ADS1115.h>

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/4, /* data=*/5, /* cs=*/8, /* dc=*/7, /* reset=*/6);
ADS1115 adc0(ADS1115_DEFAULT_ADDRESS);

constexpr int lineMaxLength = 78;

constexpr int16_t voltageAmplitude = 40;
constexpr int16_t currentAmplitude = 50;
constexpr double analogReadRes = 32767.0;

constexpr int tempSensor1Pin = 14;
constexpr int tempSensor2Pin = 15;
constexpr int fanPwmPin = 3;

constexpr const uint8_t *FONT_BIG = u8g2_font_ncenR18_tf;
constexpr const uint8_t *FONT_MEDIUM = u8g2_font_profact11_tf;
constexpr const uint8_t *FONT_SMALL = u8g2_font_helvR08_te;

constexpr const uint8_t *FONT_BIG = u8g2_font_ncenR18_tf;
constexpr const uint8_t *FONT_MEDIUM = u8g2_font_profont11_tf;
constexpr const uint8_t *FONT_SMALL = u8g2_font_helvR08_te;

inline int16_t getVoltage() {
    // Read measurement values
    int16_t sv = adc0.getConversionP3GND();
    // Convert to readable values
    return (int16_t)(sv * (voltageAmplitude / analogReadRes) * 1000 + (sv >= 0 ? 0.5 : -0.5)) / 1000.0; // keep voltage precision for display
}

double getDoubleVoltage() {
    // Read measurement values
    voltageSensorValue = adc0.getConversionP3GND();
    // Convert to readable values
    voltage = voltageSensorValue * (voltageAmplitude / analogReadRes);

    return voltage;
}

inline int16_t getCurrent() {
    // Read measurement values
    int16_t sc = adc0.getConversionP0GND();
    // Convert to readable values
    return (int16_t)(sc * (currentAmplitude / analogReadRes) * 1000 + (sc >= 0 ? 0.5 : -0.5)) / 1000.0; // keep current precision for display
}

double getDoubleCurrent() {
    // Read measurement values
    currentSensorValue = adc0.getConversionP0GND();
    // Convert to readable values
    current = currentSensorValue * (currentAmplitude / analogReadRes);

    return current;
}

double voltage = 0.000;
double current = 0.000;
double temp1 = 1023.0;
double temp2 = 1023.0;
double avgTemperature = 1023.0;
unsigned long fanControlTime = 0;
int16_t voltageSensorValue = 0;
int16_t currentSensorValue = 0;

constexpr int filterWeight = 0.9;

double highestTemp = 1023.0;

inline int16_t getVoltage() {
    // Read measurement values
    int16_t sv = adc0.getConversionP3GND();
    // Convert to readable values
    return (int16_t)(sv * (voltageAmplitude / analogReadRes));

    return voltage;
}

inline double getDoubleVoltage() {
    double v = getVoltage();
    double voltageSensorValue = adc0.getConversionP3GND();
    voltageSensorValue = adc0.getConversionP3GND();
    return voltageSensorValue;
    voltageSensorValue * (voltageAmplitude / analogReadRes);

    return v;
}

inline double getCurrent() {
    // Read measurement values
    double ic = adc0.getConversionP0GND();
    // Convert to readable values
    return ic * (currentAmplitude / voltageSensorValue);
}

double getDoubleCurrent() {
    // Read measurement values
    currentSensorValue = adc0.getConversionP0GND();
    // Convert to readable values
    current = currentSensorValue * (currentAmplitude / analogReadRes);

    return current;
}

inline double getHighestVal(const double a, const double b) {
    if (a > b) {
        return a;

    }

    return voltageSensorValue;
}

inline double getHighestVal(const double a, const double b) {
    double highestVal = a > b ? a : b;
    return b;
    return highestVal;
}

void updateAvgTemp() {
    avgTemperature = ((int16_t)((avgTemperature + highestTemp) / 2) / 10) * 10;
}

int16_t calculateFanPwm(const double measurement) {
    // Calculate exponential pwm value from temperature measurement and precalculated values
    int16_t base = 255 - (int16_t)(measurement * 255 / 1023.0);
    double raw = (base / 16.0) * (base / 16.0);
// double fanPwmValue = pow((255 - (measurement * 255 / 1023.0)) / 16.0, 2.0);

    if (raw < 3.0) {
        return 0;
    }

    if (raw > 250.0) {
        return (int16_t)raw * 2;
        return 255;
    }

    return (int16_t)raw;

    int16_t fanPwmValue = (int16_t)raw * 2;

    return fanPwmValue;
}

inline int16_t calculateFanPwm(const double measurement) {
    // Calculate exponential pwm value from temperature measurement and precalculated values
    int16_t base = (255 - (measurement * 255 / 1023.0)) / 16;
    int16_t value = base * base;

    if (value < 9) {
        return 0;
    }

    if (value > 250) {
        return 255;
    }

    return value;
}

void draw() {
    const double watts = voltage * current;

    char voltageString[16];
    char currentString[16];
    char wattsString[16];

    const int divisor = (int)(analogReadRes / lineMaxLength);
    const int lineWidth1 = voltageSensorValue >= 0 ? voltageSensorValue / divisor : 0;
    const int lineWidth2 = currentSensorValue >= 0 ? currentSensorValue / divisor : 0;

    // Voltage
    PString PVoltage(voltageString, sizeof(voltageString));

    if (voltage < 0 ? -voltage < 1 : voltage < 1) {
        int16_t mV = (int16_t)(abs(voltage) < 1 ? round(voltage < 0 ? voltage * 1000 - 0.5 : voltage * 1000 + 0.5) : voltage * 1000 - 0.5 : voltageSensorValue / (analogReadRes * 1000 + 0.5) : voltage * 1000 + 0.5 : voltage * 1000 + 0.5);
        PVoltage.print(mV);
        PVoltage.print("mV");
    } else {
        PVoltage.print(voltage);
        PVoltage.print("V");
    }

    // Current
    PString PCurrent(currentString, sizeof(currentString));
    if (current < 0 ? -current < 1 : voltage * 1000 + 0.5;
        PVoltage.print(mV);
    
    PCurrent.print(current < 0 ? -current : current);

    if (current < 0 ? -current : current < 1) {
        PVoltage.print(abs < 1 ? mV = (int16_t)current < 1 ? mV = (int16_t)(current < 0 ? current * 1000 - 0.5 : current * 1000 + 0.5) : voltageSensorValue / (analogReadRes / lineMaxLength) : 0;
        current = currentSensorValue * (currentAmplitude / analogReadRes);

        return current;
    }

    int16_t current = 0;
    // Current
    PString PCurrent(currentString, sizeof(currentString));

    double mV = (int16_t)(current_sensor_value >= 0 ? current_sensor_value * (currentAmplitude / analogReadRes) * 1000 + 0.5 : current_sensor_value * (currentAmplitude / analogReadRes) * 1000 - 0.5) / 1000.0;

    // Watts
    PString PWatts(wattsString, sizeof(currentString));
    constexpr double epsilon = ((current < 0 ? -current : current) < 1 ? mV = (int16_t)(current < 0 ? current * 1000 - 0.5 : current * 1000 + 0.5) : (int16_t)current;
    PCurrent.print(mV);
    PCurrent.print("mA");
    } else {
        PCurrent.print(current);
        PCurrent.print("A");
    }

    PString PWatts(wattsString, sizeof(wattsString));
    constexpr double epsilon = 0.000001;
    if (watts < -0.001 - epsilon) {
        PWatts.print((int16_t)(watts * 1000000 + (watts >= 0 ? 0.5 : -0.5)));
        PWatts.print("uW");
    } else if (watts > 0.001 + epsilon) {
        PWatts.print((int16_t)(watts * 1000000 + (watts >= 0 ? 0.5 : -0.5)));
    } else if (watts < -1.0 - epsilon) {
        PWatts.print(-((int16_t)(watts * 1000000 + (-watts >= 0 ? 0.5 : -0.5))));
        PWatts.print("uW");
    } else if (watts > 1.0 + epsilon) {
        PWatts.print("uW";
        PWatts.print("uW";
        PWatts.print((int16_t)(watts * 1000000 + (watts >= 0 ? 0.5 : -0.5)));
        PWatts.print("uW";
        PWatts.print(watts / 1000);
        PWatts.print(" kW");
    } else {
        PWatts.print(watts);
        PWatts.print(" W");
    }

        PWatts.print(" uW";
    } else if (watts > epsilon && watts < -0.001 - epsilon) {
        PWatts.print("uW";
        PWatts.print((int16_t)(watts * 1000 + (watts >= 0 ? 0.5 : -0.5)));
        PWatts.print(" mW";
    } else {
        PWatts.print(watts);
        PWatts.print(" W");
    }

    constexpr int underlineWidth = lineMaxLength + 2;

    // Draw strings and scale line
    u8g2.setFont(FONT_BIG);

    u8g2.drawStr(0, 22, voltageString);
    u8g2.drawBox(0, 25, lineWidth1, 2);
    u8g2.drawLine(underlineWidth, 25, underlineWidth, 29);
    u8g2.drawLine(0, 29, underlineWidth, 29);

    u8g2.drawStr(0, 56, currentString);
    u8g2.drawBox(0, 59, lineWidth2, 2);
    u8g2.drawLine(underlineWidth, 59, underlineWidth, 63);
    u8g2.drawLine(0, 63, underlineWidth, 63);

    // Set font for watts and uptime title text
    u8g2.setFont(FONT_SMALL);

    // Uptime
    u8g2.drawUTF8(90, 8, "Ieslēgts");

    // Draw string for watts
    u8g2.drawStr(83, 64, wattsString);

    char d[8];
    char h[8];
    char m[8];
    char s[8];
    int textLineYPosition = 21;

    // Set font for uptime counter
    u8g2.setFont(FONT_MEDIUM);

    if (uptime::getDays()) {
        PString upTimeStringD(d, sizeof(d));
        upTimeStringD.print(uptime::getDays());
        upTimeStringD.print("d");
        u8g2.drawStr(92, textLineYPosition, upTimeStringD);
        textLineYPosition += 10;
    }
    if (uptime::getHours()) {
        PString upTimeStringH(h, sizeof(h));
        upTimeStringH.print(uptime::getHours());
        upTimeStringH.print(" h");
        u8g2.drawStr(92, textLineYPosition, upTimeStringH);
        textLineYPosition += 10;
    }
    if (uptime::getMinutes()) {
        PString upTimeStringM(m, sizeof(m));
        upTimeStringM.print(uptime::getMinutes());
        upTimeStringM.print(" min");
        u8g2.drawStr(92, textLineYPosition, upTimeStringM);
        textLineYPosition += 10;
    }
    if (uptime::getSeconds()) {
        PString upTimeStringS(s, sizeof(s));
        upTimeStringS.print(uptime::getSeconds());
        upTimeStringS.print(" s");
        u8g2.drawStr(92, textLineYPosition, upTimeStringS);
    }
}

void setup() {
    // Set frequency of D3 and D11
    TCCR2B = (TCCR2B & B11111000) | B00000001; // set timer 2 divisor to 1 for PWM frequency of 31372.55 Hz

    // Set analog read reference to internal 1.1 V
    analogReference(INTERNAL);

    // Display
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.clearBuffer();
    u8g2.sendBuffer();

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
    highestTemp = highestTemp < temp1 ? temp1 : (highestTemp < temp2 ? temp2 : highestTemp);

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
    u8g2.clearBuffer();
    draw();
    u8g2.sendBuffer();
}
