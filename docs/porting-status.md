# Porting Status Tracker

## Port Baseline

- Primary source tree: `adafruit/Adafruit_SHT4X`

## Status Legend

- `todo`: not ported yet
- `in_progress`: implementation in progress
- `ported`: implemented in C++

## Tracking Table

| Area | Arduino reference | C++ location | Status | Notes |
| --- | --- | --- | --- | --- |
| Reset and startup flow | `Adafruit_SHT4x.cpp` | `src/sht4x.cpp` | ported | `initialize()` preserves reset-plus-serial validation. |
| Serial read and CRC checks | `Adafruit_SHT4x.cpp` | `src/sht4x.cpp` | ported | Original CRC-8 polynomial and response layout preserved. |
| Precision and heater command selection | `Adafruit_SHT4x.cpp` | `src/sht4x.cpp` | ported | Delay timings match the original driver. |
| Temperature and humidity conversion | `Adafruit_SHT4x.cpp` | `src/sht4x.cpp` | ported | Conversion formulas and humidity clamp preserved. |
| Linux I2C backend | `n/a` | `src/hardware.cpp` | ported | Uses `/dev/i2c-*` through `i2c-dev`. |
| Testable mock transport | `n/a` | `include/sht4x/hardware.hpp` | ported | Enables deterministic unit tests without hardware. |
| Raspberry Pi build presets | `n/a` | `CMakePresets.json` | ported | Mirrors the structure used in the BME690 reference project. |

## Exit Criteria

1. All rows are at least `ported`.
2. Native `cmake` build and tests pass.
3. Raspberry Pi runtime is documented and ready for hardware validation.
