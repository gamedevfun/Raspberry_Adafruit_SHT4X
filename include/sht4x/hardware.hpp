#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace sht4x {

class HardwareInterface {
public:
    virtual ~HardwareInterface() = default;
    virtual void write(std::uint8_t address, std::span<const std::uint8_t> bytes) = 0;
    virtual std::vector<std::uint8_t> read(std::uint8_t address, std::size_t length) = 0;
    virtual void delay_ms(std::uint32_t period_ms) = 0;
};

class MockSht4xDevice : public HardwareInterface {
public:
    void write(std::uint8_t address, std::span<const std::uint8_t> bytes) override;
    std::vector<std::uint8_t> read(std::uint8_t address, std::size_t length) override;
    void delay_ms(std::uint32_t period_ms) override;

    void set_response(std::uint8_t command, std::span<const std::uint8_t> response);
    std::uint8_t last_command() const;
    std::uint8_t last_address() const;
    std::uint32_t last_delay_ms() const;
    std::uint32_t delay_call_count() const;
    const std::vector<std::uint8_t> &command_history() const;

private:
    std::uint8_t last_address_{0};
    std::uint8_t last_command_{0};
    std::uint32_t last_delay_ms_{0};
    std::uint32_t delay_call_count_{0};
    std::vector<std::uint8_t> history_;
    std::vector<std::vector<std::uint8_t>> responses_{256};
};

#ifdef SHT4X_ENABLE_LINUX_I2C
class LinuxI2cDev : public HardwareInterface {
public:
    explicit LinuxI2cDev(int bus = 1, const std::string &prefix = "/dev/i2c-");
    ~LinuxI2cDev() override;

    void write(std::uint8_t address, std::span<const std::uint8_t> bytes) override;
    std::vector<std::uint8_t> read(std::uint8_t address, std::size_t length) override;
    void delay_ms(std::uint32_t period_ms) override;

private:
    void select_device(std::uint8_t address);

    int fd_{-1};
    std::string path_;
    std::uint8_t selected_address_{0xFF};
};
#endif

}  // namespace sht4x
