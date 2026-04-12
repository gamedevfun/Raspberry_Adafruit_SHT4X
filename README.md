# Raspberry Pi SHT4X C++ Port

<a href="https://www.adafruit.com/product/4885"><img src="assets/board.jpg?raw=true" width="500px"></a>

Modern C++20 port of the original `adafruit/Adafruit_SHT4X` Arduino driver for Raspberry Pi and other Linux systems with `i2c-dev`.

## Why This Fork Exists

This fork was created to make the SHT4X driver usable in native Raspberry Pi C++ projects without Arduino runtime dependencies.

The goals of the fork are:

- preserve the original Adafruit command flow and conversion formulas
- expose a Linux-friendly C++ API with a clean hardware abstraction
- support native Linux builds and Raspberry Pi cross-compilation with CMake
- keep attribution and licensing from the upstream Adafruit project explicit

## License And Upstream Attribution

This repository is a derivative port of the Adafruit SHT4X library.

- Upstream project: `https://github.com/adafruit/Adafruit_SHT4X`
- Original authorship: Limor Fried / Ladyada for Adafruit Industries
- Original and continued license: BSD 3-Clause
- Original license text is retained in `license.txt`
- Original upstream redistribution notice is retained verbatim in `NOTICE`

The new Linux-specific code in this fork is distributed under the same BSD 3-Clause terms so the repository has one clear license story. This repository keeps the upstream BSD license text and the original upstream notice text together so redistribution requirements are explicit.

## Features

- Linux `i2c-dev` backend for Raspberry Pi
- mock transport for unit tests
- C++20 static library with install rules
- native and Raspberry Pi cross-build presets
- example programs for serial readout and continuous measurements

## Requirements

- CMake >= 3.16
- C++20 compiler
- Linux runtime for live hardware access through `/dev/i2c-*`

Cross-compilation is intended for Linux or WSL hosts using GNU cross-compilers and a Raspberry Pi sysroot.

## Build

Native debug build:

```bash
cmake --preset native-debug
cmake --build --preset native-debug
ctest --test-dir build --output-on-failure
```

Without the Linux I2C backend:

```bash
cmake -S . -B build -DSHT4X_ENABLE_LINUX_I2C=OFF
cmake --build build
```

## Cross-Compile For Raspberry Pi

Available presets:

- `rpi-aarch64-release`
- `rpi-armhf-release`

Install cross-compilers on the host:

```bash
sudo apt install g++-aarch64-linux-gnu-12 gcc-aarch64-linux-gnu-12
sudo apt install g++-arm-linux-gnueabihf gcc-arm-linux-gnueabihf
sudo apt install rsync
```

Populate a sysroot from the target Pi:

```bash
rsync -a --delete pi@<pi-ip>:/usr .sysroot/
rsync -a --delete pi@<pi-ip>:/lib .sysroot/
rsync -a --delete pi@<pi-ip>:/opt .sysroot/
```

Then configure and build:

```bash
cmake --preset rpi-aarch64-release
cmake --build --preset rpi-aarch64-release
```

```bash
cmake --preset rpi-armhf-release
cmake --build --preset rpi-armhf-release
```

## Run On Raspberry Pi

Enable I2C:

```bash
sudo raspi-config nonint do_i2c 0
```

Build and run an example locally on the Pi:

```bash
cmake --preset native-debug
cmake --build --preset native-debug
./build/sht4x_read_once
```

## Repository Layout

- `include/` public headers
- `src/` C++ driver and hardware implementation
- `examples/` Raspberry Pi example programs
- `tests/` C++ tests
- `docs/` porting notes and structure

## API Sketch

```cpp
#include "sht4x/sht4x.hpp"

int main() {
    sht4x::SHT4x sensor;
    sensor.initialize();
    const auto measurement = sensor.measure();
}
```
