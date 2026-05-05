#include "hw/display.h"

#include <U8g2lib.h>
#include <uptime.h>

static U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(
    U8G2_R0, /* clock */ 4, /* data */ 5, /* cs */ 8, /* dc */ 7, /* reset */ 6);

static const uint8_t* fontBig = u8g2_font_ncenR18_tf;
static const uint8_t* fontMedium = u8g2_font_profont11_tf;
static const uint8_t* fontSmall = u8g2_font_helvR08_te;

static void draw_value_bar(const char* str, int16_t rawValue, int yBase)
{
    const int divisor = static_cast<int>(adcResolution / lineMaxLength);
    const int16_t lineWidth = rawValue >= 0 ? rawValue / divisor : 0;

    const int underlineWidth = lineMaxLength + 2;

    u8g2.setFont(fontBig);
    u8g2.drawStr(0, yBase, str);

    u8g2.drawBox(0, yBase + 3, lineWidth, 2);
    u8g2.drawLine(underlineWidth, yBase + 3, underlineWidth, yBase + 7);
    u8g2.drawLine(0, yBase + 7, underlineWidth, yBase + 7);
}

void display_hw_init()
{
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}

static void format_int(char* buf, int32_t val, const char* unit)
{
    ltoa(val, buf, 10);
    strcat(buf, unit);
}

static void format_voltage_current(char* buf, double val, const char* smallUnit, const char* bigUnit)
{
    if ((val >= 0 ? val : -val) < 1.0 - epsilon)
    {
        format_int(buf, static_cast<int32_t>(val * 1000.0 + (val >= 0 ? 0.5 : -0.5)), smallUnit);
    }
    else
    {
        dtostrf(val, 1, 2, buf);
        strcat(buf, bigUnit);
    }
}

void display_hw_render(const AppState* s)
{
    char voltageStr[16], currentStr[16], wattsStr[16], tmpBuf[16];

    // Voltage
    format_voltage_current(voltageStr, s->voltage, "mV", "V");

    // Current
    format_voltage_current(currentStr, s->current, "mA", "A");

    // Watts
    {
        double wAbs = s->watts >= 0 ? s->watts : -s->watts;
        if (wAbs > wattsHighThreshold + epsilon)
        {
            format_int(wattsStr, static_cast<int32_t>(s->watts / 1000.0 + (s->watts >= 0 ? 0.5 : -0.5)), " kW");
        }
        else if (wAbs >= wattsLowThreshold - epsilon)
        {
            format_int(wattsStr, static_cast<int32_t>(s->watts + (s->watts >= 0 ? 0.5 : -0.5)), " W");
        }
        else if (wAbs >= 0.001 - epsilon)
        {
            format_int(wattsStr, static_cast<int32_t>(s->watts * 1000.0 + (s->watts >= 0 ? 0.5 : -0.5)), " mW");
        }
        else
        {
            format_int(wattsStr, static_cast<int32_t>(s->watts * 1000000.0 + (s->watts >= 0 ? 0.5 : -0.5)), " uW");
        }
    }

    // Draw strings and scale line
    draw_value_bar(voltageStr, s->rawVoltage, 22);
    draw_value_bar(currentStr, s->rawCurrent, 56);

    // Set font for watts and uptime title text
    u8g2.setFont(fontSmall);

    // Uptime
    u8g2.drawUTF8(90, 8, "Iesl\u0113gts");

    // Draw string for watts
    u8g2.drawStr(83, 64, wattsStr);

    int yPos = 21;

    // Set font for uptime counter
    u8g2.setFont(fontMedium);

    if (uptime::getDays())
    {
        ultoa(uptime::getDays(), tmpBuf, 10);
        strcat(tmpBuf, "d");
        u8g2.drawStr(92, yPos, tmpBuf);
        yPos += 10;
    }
    if (uptime::getHours())
    {
        ultoa(uptime::getHours(), tmpBuf, 10);
        strcat(tmpBuf, " h");
        u8g2.drawStr(92, yPos, tmpBuf);
        yPos += 10;
    }
    if (uptime::getMinutes())
    {
        ultoa(uptime::getMinutes(), tmpBuf, 10);
        strcat(tmpBuf, " min");
        u8g2.drawStr(92, yPos, tmpBuf);
        yPos += 10;
    }
    if (uptime::getSeconds())
    {
        ultoa(uptime::getSeconds(), tmpBuf, 10);
        strcat(tmpBuf, " s");
        u8g2.drawStr(92, yPos, tmpBuf);
    }
}

void display_hw_clear()
{
    u8g2.clearBuffer();
}

void display_hw_send()
{
    u8g2.sendBuffer();
}
