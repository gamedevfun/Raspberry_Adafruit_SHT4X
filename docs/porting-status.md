# Porting Status Tracker

## Port Baseline

- Primary source tree: `adafruit/Adafruit_SHT4X`
- Behavioral parity reference for command coverage and timing: `Sensirion/embedded-i2c-sht4x`

## Status Legend

- `todo`: not ported yet
- `in_progress`: implementation in progress
- `ported`: implemented in C++

## Tracking Table

| Area | Arduino reference | C++ location | Status | Notes |
| --- | --- | --- | --- | --- |
| Reset and startup flow | `Adafruit_SHT4x.cpp`, `sht4x_i2c.c` | `src/sht4x.cpp` | ported | Reset timing and serial-read startup flow align with the combined references. |
| Serial read and CRC checks | `Adafruit_SHT4x.cpp` | `src/sht4x.cpp` | ported | Original CRC-8 polynomial and response layout preserved. |
| Precision and heater command selection | `Adafruit_SHT4x.cpp`, `sht4x_i2c.c` | `src/sht4x.cpp` | ported | Command timings cover all Sensirion one-shot variants. |
| Temperature and humidity conversion | `Adafruit_SHT4x.cpp` | `src/sht4x.cpp` | ported | Conversion formulas and humidity clamp preserved. |
| Raw tick access | `sht4x_i2c.c` | `include/sht4x/sht4x.hpp`, `src/sht4x.cpp` | ported | Raw temperature and humidity ticks are exposed for parity and custom conversion. |
| Linux I2C backend | `n/a` | `src/hardware.cpp` | ported | Uses `/dev/i2c-*` through `i2c-dev`. |
| Testable mock transport | `n/a` | `include/sht4x/hardware.hpp` | ported | Enables deterministic unit tests without hardware. |
| Raspberry Pi build presets | `n/a` | `CMakePresets.json` | ported | Mirrors the structure used in the BME690 reference project. |

## Exit Criteria

1. All rows are at least `ported`.
2. Native `cmake` build and tests pass.
3. Raspberry Pi runtime is documented and ready for hardware validation.
