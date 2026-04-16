#include "sht4x/sht4x.hpp"

#include <chrono>
#include <exception>
#include <iomanip>
#include <iostream>
#include <thread>

int main() {
    try {
        sht4x::SHT4x sensor(nullptr, sht4x::address_44);

        sensor.soft_reset();
        const auto serial = sensor.read_serial();

        std::cout << "serial=0x" << std::hex << std::uppercase << serial << std::dec << '\n';

        while (true) {
            const auto measurement = sensor.measure(sht4x::Precision::low, sht4x::Heater::none);
            std::cout << std::fixed << std::setprecision(2)
                      << "temperature=" << measurement.temperature_c << " C, "
                      << "humidity=" << measurement.humidity_percent << " %RH\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
