# siggen-promini

AD9833-based signal generator on an Arduino Pro Mini 3.3V/8MHz.

A PlatformIO/Arduino project that drives an AD9833 DDS module to generate waveforms, controlled via a rotary encoder and a 1.8" ST7735 colour TFT using the ArduinoMenu library.

## Hardware

- Arduino Pro Mini 3.3V/8MHz (ATmega328P)
- AD9833 DDS breakout (hardware SPI)
- 1.8" ST7735 TFT display (software SPI)
- Rotary encoder with push-button

## Pin Assignments

| Function | Pin |
|----------|-----|
| AD9833 FSYNC | SS (hardware SPI) |
| TFT CS | 9 |
| TFT DC | 7 |
| TFT MOSI | 6 |
| TFT SCLK | 5 |
| TFT RST | 8 |
| Encoder A | 2 |
| Encoder B | 3 |
| Encoder Button | 4 |

## Build

Requires [PlatformIO](https://platformio.org/).

```bash
pio run -t upload
```
