#include "sht4x/sht4x.hpp"

#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using TestFn = std::function<void()>;

void expect(bool condition, const std::string &message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void expect_close(double actual, double expected, double epsilon, const std::string &message) {
    if (std::fabs(actual - expected) > epsilon) {
        throw std::runtime_error(message + " actual=" + std::to_string(actual) + " expected=" + std::to_string(expected));
    }
}

std::uint8_t crc8(const std::uint8_t *data, std::size_t length) {
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

std::vector<std::uint8_t> make_payload(std::uint16_t first, std::uint16_t second) {
    std::vector<std::uint8_t> payload = {
        static_cast<std::uint8_t>((first >> 8) & 0xFF),
        static_cast<std::uint8_t>(first & 0xFF),
        0,
        static_cast<std::uint8_t>((second >> 8) & 0xFF),
        static_cast<std::uint8_t>(second & 0xFF),
        0,
    };
    payload[2] = crc8(payload.data(), 2);
    payload[5] = crc8(payload.data() + 3, 2);
    return payload;
}

std::shared_ptr<sht4x::MockSht4xDevice> make_mock() {
    return std::make_shared<sht4x::MockSht4xDevice>();
}

void test_initialize_and_serial() {
    auto mock = make_mock();
    mock->set_response(sht4x::command_read_serial, make_payload(0x1234, 0xABCD));

    sht4x::SHT4x sensor(mock);
    sensor.initialize();
    const auto serial = sensor.read_serial();

    expect(serial == 0x1234ABCDU, "Expected 32-bit serial to match upstream layout");
    expect(mock->command_history().size() == 3, "Expected reset plus two serial reads");
    expect(mock->command_history()[0] == sht4x::command_soft_reset, "Expected first command to be soft reset");
    expect(mock->last_delay_ms() == 10, "Expected serial read delay to be 10 ms");
}

void test_measurement_conversion_and_clamp() {
    auto mock = make_mock();
    mock->set_response(sht4x::command_read_serial, make_payload(0x1000, 0x2000));
    mock->set_response(sht4x::command_no_heat_high_precision, make_payload(0x6666, 0x0000));

    sht4x::SHT4x sensor(mock);
    sensor.initialize();
    const auto measurement = sensor.measure();

    expect(mock->last_command() == sht4x::command_no_heat_high_precision, "Expected default high precision command");
    expect(mock->last_delay_ms() == 10, "Expected default high precision delay");
    expect_close(measurement.temperature_c, -45.0 + 175.0 * 0x6666 / 65535.0, 0.0001, "Temperature conversion mismatch");
    expect_close(measurement.humidity_percent, 0.0, 0.0001, "Humidity should clamp to zero");
}

void test_precision_and_heater_modes() {
    auto mock = make_mock();
    mock->set_response(sht4x::command_read_serial, make_payload(0x1000, 0x2000));
    mock->set_response(sht4x::command_no_heat_low_precision, make_payload(0x4000, 0x8000));
    mock->set_response(sht4x::command_medium_heat_100ms, make_payload(0x4000, 0x8000));

    sht4x::SHT4x sensor(mock);
    sensor.initialize();

    sensor.set_precision(sht4x::Precision::low);
    (void)sensor.measure();
    expect(mock->last_command() == sht4x::command_no_heat_low_precision, "Expected low precision command");
    expect(mock->last_delay_ms() == 2, "Expected low precision delay");

    sensor.set_heater(sht4x::Heater::medium_100ms);
    (void)sensor.measure();
    expect(mock->last_command() == sht4x::command_medium_heat_100ms, "Expected heater command to override precision");
    expect(mock->last_delay_ms() == 110, "Expected heater delay");
}

void test_crc_failure_throws() {
    auto mock = make_mock();
    const std::vector<std::uint8_t> invalid_payload = {0x12, 0x34, 0x00, 0xAB, 0xCD, 0x00};
    mock->set_response(sht4x::command_read_serial, invalid_payload);

    bool threw = false;
    try {
        sht4x::SHT4x sensor(mock);
        sensor.initialize();
    } catch (const std::runtime_error &) {
        threw = true;
    }

    expect(threw, "Expected initialize to fail on CRC mismatch");
}

}  // namespace

int main() {
    const std::vector<std::pair<std::string, TestFn>> tests = {
        {"initialize_and_serial", test_initialize_and_serial},
        {"measurement_conversion_and_clamp", test_measurement_conversion_and_clamp},
        {"precision_and_heater_modes", test_precision_and_heater_modes},
        {"crc_failure_throws", test_crc_failure_throws},
    };

    for (const auto &[name, test] : tests) {
        try {
            test();
        } catch (const std::exception &error) {
            std::cerr << "FAIL " << name << ": " << error.what() << '\n';
            return 1;
        }
    }

    std::cout << "PASS " << tests.size() << " tests\n";
    return 0;
}
