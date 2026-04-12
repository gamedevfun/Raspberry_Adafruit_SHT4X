/*
 * Derived Raspberry Pi/Linux port of the Adafruit SHT4X library.
 * Original work: Limor Fried / Ladyada for Adafruit Industries.
 * License: BSD 3-Clause, see license.txt.
 * Upstream notice text retained verbatim in NOTICE.
 */

#include "sht4x/hardware.hpp"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <thread>

#ifdef SHT4X_ENABLE_LINUX_I2C
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace sht4x {

void MockSht4xDevice::write(std::uint8_t, std::span<const std::uint8_t> bytes) {
    if (bytes.empty()) {
        throw std::runtime_error("mock write requires at least one command byte");
    }
    last_command_ = bytes.front();
    history_.push_back(last_command_);
}

std::vector<std::uint8_t> MockSht4xDevice::read(std::uint8_t, std::size_t length) {
    std::vector<std::uint8_t> result(length, 0);
    const auto &configured = responses_.at(last_command_);
    const auto copy_length = std::min(length, configured.size());
    std::copy_n(configured.begin(), copy_length, result.begin());
    return result;
}

void MockSht4xDevice::delay_ms(std::uint32_t period_ms) {
    last_delay_ms_ = period_ms;
    ++delay_call_count_;
}

void MockSht4xDevice::set_response(std::uint8_t command, std::span<const std::uint8_t> response) {
    responses_.at(command) = std::vector<std::uint8_t>(response.begin(), response.end());
}

std::uint8_t MockSht4xDevice::last_command() const {
    return last_command_;
}

std::uint32_t MockSht4xDevice::last_delay_ms() const {
    return last_delay_ms_;
}

std::uint32_t MockSht4xDevice::delay_call_count() const {
    return delay_call_count_;
}

const std::vector<std::uint8_t> &MockSht4xDevice::command_history() const {
    return history_;
}

#ifdef SHT4X_ENABLE_LINUX_I2C
namespace {

std::runtime_error system_error(const std::string &context) {
    return std::runtime_error(context + ": " + std::strerror(errno));
}

}

LinuxI2cDev::LinuxI2cDev(int bus, const std::string &prefix) : path_(prefix + std::to_string(bus)) {
    fd_ = ::open(path_.c_str(), O_RDWR);
    if (fd_ < 0) {
        throw system_error("open i2c device");
    }
}

LinuxI2cDev::~LinuxI2cDev() {
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

void LinuxI2cDev::select_device(std::uint8_t address) {
    if (selected_address_ == address) {
        return;
    }

    if (ioctl(fd_, I2C_SLAVE, address) < 0) {
        throw system_error("select i2c slave");
    }
    selected_address_ = address;
}

void LinuxI2cDev::write(std::uint8_t address, std::span<const std::uint8_t> bytes) {
    select_device(address);
    if (::write(fd_, bytes.data(), static_cast<unsigned int>(bytes.size())) != static_cast<int>(bytes.size())) {
        throw system_error("write i2c bytes");
    }
}

std::vector<std::uint8_t> LinuxI2cDev::read(std::uint8_t address, std::size_t length) {
    select_device(address);
    std::vector<std::uint8_t> buffer(length, 0);
    if (::read(fd_, buffer.data(), static_cast<unsigned int>(length)) != static_cast<int>(length)) {
        throw system_error("read i2c bytes");
    }
    return buffer;
}

void LinuxI2cDev::delay_ms(std::uint32_t period_ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(period_ms));
}
#endif

}  // namespace sht4x
