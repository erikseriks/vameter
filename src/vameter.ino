#include <uptime_formatter.h>
#include <uptime.h>
#include <U8g2lib.h>
#include <PString.h>
#include <ADS1115.h>

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/4, /* data=*/5, /* cs=*/8, /* dc=*/7, /* reset=*/6);
ADS1115 adc0(ADS1115_DEFAULT_ADDRESS);

constexpr int lineMaxLength = 78;

constexpr double voltageAmplitude = 40.0;
constexpr double currentAmplitude = 50.0;
constexpr double analogReadRes = 32767.0;

double voltage = 0.000;
double current = 0.000;

double temp1 = 1023.0;
double temp2 = 1023.0;
double avgTemperature = 1023.0;
double highestTemp = 1023.0;

unsigned long fanControlTime = 0;

int16_t voltageSensorValue = 0;
int16_t currentSensorValue = 0;

double fanPwmValue = 255;

constexpr int filterWeight = 0.9;

constexpr int tempSensor1Pin = 14;
constexpr int tempSensor2Pin = 15;
constexpr int fanPwmPin = 3;

constexpr const uint8_t *FONT_BIG = u8g2_font_ncenR18_tf;
constexpr const uint8_t *FONT_MEDIUM = u8g2_font_profont11_tf;
constexpr const uint8_t *FONT_SMALL = u8g2_font_helvR08_te;

inline double getVoltage() {
    int16_t sv = adc0.getConversionP3GND();
    voltageSensorValue = sv;
    return sv * (voltageAmplitude / analogReadRes);
}

inline double getCurrent() {
    int16_t sc = adc0.getConversionP0GND();
    currentSensorValue = sc;
    return sc * (currentAmplitude / analogReadRes);
}

inline double getHighestVal(const double a, const double b) {
    return a > b ? a : b;
}

inline int32_t roundSigned(double x) {
    return (int32_t)(x + (x >= 0 ? 0.5 : -0.5));
}

void updateAvgTemp() {
    avgTemperature = ((int32_t)((avgTemperature + highestTemp) / 2) / 10) * 10;
}

inline uint8_t calculateFanPwm(const double measurement) {
    int16_t base = (int16_t)(255.0 - (measurement * 255.0 / 1023.0));
    int16_t value = (base / 16) * (base / 16);

    if (value < 9) {
        return 0;
    }
    if (value > 250) {
        return 255;
    }
    return (uint8_t)value;
}

void draw() {
    constexpr double epsilon = 0.000001;
    const double watts = voltage * current;

    char voltageString[16];
    char currentString[16];
    char wattsString[16];

    const int divisor = (int)(analogReadRes / lineMaxLength);
    const int16_t lineWidth1 = voltageSensorValue >= 0 ? voltageSensorValue / divisor : 0;
    const int16_t lineWidth2 = currentSensorValue >= 0 ? currentSensorValue / divisor : 0;

    // Voltage
    PString PVoltage(voltageString, sizeof(voltageString));
    double vAbs = voltage >= 0 ? voltage : -voltage;
    if (vAbs < 1) {
        PVoltage.print(roundSigned(voltage * 1000));
        PVoltage.print("mV");
    } else {
        PVoltage.print(voltage);
        PVoltage.print("V");
    }

    // Current
    PString PCurrent(currentString, sizeof(currentString));
    double cAbs = current >= 0 ? current : -current;
    if (cAbs < 1) {
        PCurrent.print(roundSigned(current * 1000));
        PCurrent.print("mA");
    } else {
        PCurrent.print(current);
        PCurrent.print("A");
    }

    // Watts
    PString PWatts(wattsString, sizeof(wattsString));
    double wAbs = watts >= 0 ? watts : -watts;
    if (wAbs > 1000.0 + epsilon) {
        PWatts.print((int32_t)(watts / 1000.0 + (watts >= 0 ? 0.5 : -0.5)));
        PWatts.print(" kW");
    } else if (wAbs >= 1.0 - epsilon) {
        PWatts.print((int32_t)(watts + (watts >= 0 ? 0.5 : -0.5)));
        PWatts.print(" W");
    } else if (wAbs >= 0.001 - epsilon) {
        PWatts.print(roundSigned(watts * 1000));
        PWatts.print(" mW");
    } else {
        PWatts.print(roundSigned(watts * 1000000));
        PWatts.print(" uW");
    }

    constexpr int underlineWidth = lineMaxLength + 2;

    u8g2.setFont(FONT_BIG);
    u8g2.drawStr(0, 22, voltageString);
    u8g2.drawBox(0, 25, lineWidth1, 2);
    u8g2.drawLine(underlineWidth, 25, underlineWidth, 29);
    u8g2.drawLine(0, 29, underlineWidth, 29);

    u8g2.drawStr(0, 56, currentString);
    u8g2.drawBox(0, 59, lineWidth2, 2);
    u8g2.drawLine(underlineWidth, 59, underlineWidth, 63);
    u8g2.drawLine(0, 63, underlineWidth, 63);

    u8g2.setFont(FONT_SMALL);
    u8g2.drawUTF8(90, 8, "Ieslēgts");
    u8g2.drawStr(83, 64, wattsString);

    char d[8];
    char h[8];
    char m[8];
    char s[8];
    int textLineYPosition = 21;

    u8g2.setFont(FONT_MEDIUM);

    if (uptime::getDays()) {
        PString upTimeStringD(d, sizeof(d));
        upTimeStringD.print(uptime::getDays());
        upTimeStringD.print(F("d"));
        u8g2.drawStr(92, textLineYPosition, upTimeStringD);
        textLineYPosition += 10;
    }
    if (uptime::getHours()) {
        PString upTimeStringH(h, sizeof(h));
        upTimeStringH.print(uptime::getHours());
        upTimeStringH.print(F(" h"));
        u8g2.drawStr(92, textLineYPosition, upTimeStringH);
        textLineYPosition += 10;
    }
    if (uptime::getMinutes()) {
        PString upTimeStringM(m, sizeof(m));
        upTimeStringM.print(uptime::getMinutes());
        upTimeStringM.print(F(" min"));
        u8g2.drawStr(92, textLineYPosition, upTimeStringM);
        textLineYPosition += 10;
    }
    if (uptime::getSeconds()) {
        PString upTimeStringS(s, sizeof(s));
        upTimeStringS.print(uptime::getSeconds());
        upTimeStringS.print(F(" s"));
        u8g2.drawStr(92, textLineYPosition, upTimeStringS);
    }
}

void setup() {
    TCCR2B = (TCCR2B & B11111000) | B00000001;

    analogReference(INTERNAL);

    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.clearBuffer();
    u8g2.sendBuffer();

    Wire.begin();
    adc0.initialize();
    adc0.setMode(ADS1115_MODE_CONTINUOUS);
    adc0.setGain(ADS1115_PGA_4P096);
    adc0.setRate(ADS1115_RATE_64);

    pinMode(tempSensor1Pin, INPUT);
    pinMode(tempSensor2Pin, INPUT);
    pinMode(fanPwmPin, OUTPUT);
}

void loop() {
    voltage = getVoltage();
    current = getCurrent();

    temp1 = analogRead(tempSensor1Pin);
    temp2 = analogRead(tempSensor2Pin);

    highestTemp = temp1 > temp2 ? temp1 : temp2;
    updateAvgTemp();

    if (fanControlTime < millis()) {
        analogWrite(fanPwmPin, calculateFanPwm(avgTemperature));
        fanControlTime = millis() + 1000;
    }

    uptime::calculateUptime();

    u8g2.clearBuffer();
    draw();
    u8g2.sendBuffer();
}
