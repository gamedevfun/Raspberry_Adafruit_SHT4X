/*
 * Derived Raspberry Pi/Linux port of the Adafruit SHT4X library.
 * Original work: Limor Fried / Ladyada for Adafruit Industries.
 * License: BSD 3-Clause, see license.txt.
 * Upstream notice text retained verbatim in NOTICE.
 */

#include "sht4x/sht4x.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>

namespace sht4x {

SHT4x::SHT4x(std::shared_ptr<HardwareInterface> hardware, std::uint8_t address)
    : address_(address), hardware_(std::move(hardware)) {
    if (!hardware_) {
#ifdef SHT4X_ENABLE_LINUX_I2C
        hardware_ = std::make_shared<LinuxI2cDev>(1);
#else
        throw std::runtime_error("No hardware backend provided and Linux I2C backend is unavailable");
#endif
    }
}

void SHT4x::initialize() {
    soft_reset();
    (void)read_serial();
}

void SHT4x::soft_reset() {
    const std::array<std::uint8_t, 1> command = {command_soft_reset};
    hardware_->write(address_, command);
    hardware_->delay_ms(10);
}

std::uint32_t SHT4x::read_serial() {
    const std::array<std::uint8_t, 1> command = {command_read_serial};
    hardware_->write(address_, command);
    hardware_->delay_ms(10);

    const auto reply = hardware_->read(address_, 6);
    if (reply.size() != 6) {
        throw std::runtime_error("read_serial returned invalid response length");
    }
    if (crc8(reply.data(), 2) != reply[2] || crc8(reply.data() + 3, 2) != reply[5]) {
        throw std::runtime_error("read_serial CRC mismatch");
    }

    std::uint32_t serial = reply[0];
    serial = (serial << 8) | reply[1];
    serial = (serial << 8) | reply[3];
    serial = (serial << 8) | reply[4];
    return serial;
}

void SHT4x::set_precision(Precision precision) {
    precision_ = precision;
}

Precision SHT4x::precision() const {
    return precision_;
}

void SHT4x::set_heater(Heater heater) {
    heater_ = heater;
}

Heater SHT4x::heater() const {
    return heater_;
}

Measurement SHT4x::measure() {
    return convert_ticks(measure_ticks());
}

Measurement SHT4x::measure(Precision precision, Heater heater) {
    return convert_ticks(measure_ticks(precision, heater));
}

MeasurementTicks SHT4x::measure_ticks() {
    return measure_ticks(precision_, heater_);
}

MeasurementTicks SHT4x::measure_ticks(Precision precision, Heater heater) {
    const auto settings = resolve_settings(precision, heater);
    const std::array<std::uint8_t, 1> command = {settings.command};
    hardware_->write(address_, command);
    hardware_->delay_ms(settings.delay_ms);

    const auto reply = hardware_->read(address_, 6);
    if (reply.size() != 6) {
        throw std::runtime_error("measure returned invalid response length");
    }
    if (crc8(reply.data(), 2) != reply[2] || crc8(reply.data() + 3, 2) != reply[5]) {
        throw std::runtime_error("measurement CRC mismatch");
    }

    MeasurementTicks ticks;
    ticks.temperature_ticks = static_cast<std::uint16_t>((reply[0] << 8) | reply[1]);
    ticks.humidity_ticks = static_cast<std::uint16_t>((reply[3] << 8) | reply[4]);
    return ticks;
}

Measurement SHT4x::convert_ticks(const MeasurementTicks &ticks) {
    Measurement measurement;
    measurement.temperature_c = -45.0 + 175.0 * static_cast<double>(ticks.temperature_ticks) / 65535.0;
    measurement.humidity_percent = -6.0 + 125.0 * static_cast<double>(ticks.humidity_ticks) / 65535.0;
    measurement.humidity_percent = std::clamp(measurement.humidity_percent, 0.0, 100.0);
    return measurement;
}

SHT4x::CommandSettings SHT4x::resolve_settings(Precision precision, Heater heater) {
    switch (heater) {
        case Heater::high_1s:
            return {command_high_heat_1s, 1100};
        case Heater::high_100ms:
            return {command_high_heat_100ms, 110};
        case Heater::medium_1s:
            return {command_medium_heat_1s, 1100};
        case Heater::medium_100ms:
            return {command_medium_heat_100ms, 110};
        case Heater::low_1s:
            return {command_low_heat_1s, 1100};
        case Heater::low_100ms:
            return {command_low_heat_100ms, 110};
        case Heater::none:
            switch (precision) {
                case Precision::high:
                    return {command_no_heat_high_precision, 10};
                case Precision::medium:
                    return {command_no_heat_medium_precision, 5};
                case Precision::low:
                    return {command_no_heat_low_precision, 2};
            }
    }

    throw std::runtime_error("invalid SHT4x mode selection");
}

SHT4x::CommandSettings SHT4x::selected_settings() const {
    return resolve_settings(precision_, heater_);
}

std::uint8_t SHT4x::crc8(const std::uint8_t *data, std::size_t length) {
    constexpr std::uint8_t polynomial = 0x31;
    std::uint8_t crc = 0xFF;

    for (std::size_t index = 0; index < length; ++index) {
        crc ^= data[index];
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc & 0x80U) != 0U ? static_cast<std::uint8_t>((crc << 1U) ^ polynomial)
                                      : static_cast<std::uint8_t>(crc << 1U);
        }
    }

    return crc;
}

}  // namespace sht4x
