#include "sht4x/sht4x.hpp"

#include <exception>
#include <iomanip>
#include <iostream>

int main() {
    try {
        sht4x::SHT4x sensor;
        sensor.initialize();

        const auto serial = sensor.read_serial();
        const auto measurement = sensor.measure();

        std::cout << "serial=0x" << std::hex << std::uppercase << serial << std::dec << '\n';
        std::cout << std::fixed << std::setprecision(2)
                  << "temperature=" << measurement.temperature_c << " C\n"
                  << "humidity=" << measurement.humidity_percent << " %RH\n";
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
