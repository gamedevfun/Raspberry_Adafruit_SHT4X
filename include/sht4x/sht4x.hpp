#pragma once

#include "sht4x/constants.hpp"
#include "sht4x/hardware.hpp"

#include <cstdint>
#include <memory>

namespace sht4x {

class SHT4x {
public:
    explicit SHT4x(std::shared_ptr<HardwareInterface> hardware = nullptr, std::uint8_t address = default_address);

    void initialize();
    void soft_reset();
    std::uint32_t read_serial();

    void set_precision(Precision precision);
    Precision precision() const;
    void set_heater(Heater heater);
    Heater heater() const;

    Measurement measure();

private:
    std::uint8_t selected_command() const;
    std::uint32_t selected_delay_ms() const;
    static std::uint8_t crc8(const std::uint8_t *data, std::size_t length);

    std::uint8_t address_;
    std::shared_ptr<HardwareInterface> hardware_;
    Precision precision_{Precision::high};
    Heater heater_{Heater::none};
};

}  // namespace sht4x
