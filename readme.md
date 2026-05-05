# VA meter

> PlatformIO, USBASP

### Fast and inexpensive volt, ampere and power meter with visual scale, uptime and fan control

- ATmega328P 8-bit AVR Microcontroller with **32K** Bytes In-System Programmable Flash
- ADS1115
- Oled display (2.42" 128x64)
- Two thermistors

## Build & flash

```sh
pio run              # build
pio run -t upload    # flash via usbasp
```

## Pinout

| Component          | Pin     |
|--------------------|---------|
| OLED clock         | 4       |
| OLED data          | 5       |
| OLED CS            | 8       |
| OLED DC            | 7       |
| OLED reset         | 6       |
| Fan PWM            | 3       |
| Thermistor 1       | A0 (14) |
| Thermistor 2       | A1 (15) |
| ADS1115 voltage ch | P3      |
| ADS1115 current ch | P0      |

Timer 2 divisor set to 1 for 31372 Hz PWM. ADC reference set to internal 1.1V.
