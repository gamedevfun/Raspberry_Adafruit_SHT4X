#pragma once

#include <cstdint>

namespace sht4x {

constexpr std::uint8_t default_address = 0x44;

constexpr std::uint8_t command_no_heat_high_precision = 0xFD;
constexpr std::uint8_t command_no_heat_medium_precision = 0xF6;
constexpr std::uint8_t command_no_heat_low_precision = 0xE0;
constexpr std::uint8_t command_high_heat_1s = 0x39;
constexpr std::uint8_t command_high_heat_100ms = 0x32;
constexpr std::uint8_t command_medium_heat_1s = 0x2F;
constexpr std::uint8_t command_medium_heat_100ms = 0x24;
constexpr std::uint8_t command_low_heat_1s = 0x1E;
constexpr std::uint8_t command_low_heat_100ms = 0x15;
constexpr std::uint8_t command_read_serial = 0x89;
constexpr std::uint8_t command_soft_reset = 0x94;

enum class Precision {
    high,
    medium,
    low,
};

enum class Heater {
    none,
    high_1s,
    high_100ms,
    medium_1s,
    medium_100ms,
    low_1s,
    low_100ms,
};

struct Measurement {
    double temperature_c{0.0};
    double humidity_percent{0.0};
};

}  // namespace sht4x
