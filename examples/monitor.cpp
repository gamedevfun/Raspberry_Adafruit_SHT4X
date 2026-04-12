#include "sht4x/sht4x.hpp"

#include <chrono>
#include <exception>
#include <iomanip>
#include <iostream>
#include <thread>

int main() {
    try {
        sht4x::SHT4x sensor;
        sensor.initialize();
        sensor.set_precision(sht4x::Precision::high);
        sensor.set_heater(sht4x::Heater::none);

        while (true) {
            const auto measurement = sensor.measure();
            std::cout << std::fixed << std::setprecision(2)
                      << measurement.temperature_c << " C, "
                      << measurement.humidity_percent << " %RH\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
