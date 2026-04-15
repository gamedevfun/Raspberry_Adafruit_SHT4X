# C++ Port Structure And Boundaries

## Goal

Keep the C++ implementation production-ready for Raspberry Pi and Linux while preserving the original Adafruit sensor behavior.

## Ownership

- C++ production code:
  - `include/`
  - `src/`
  - `examples/`
  - `tests/`

## Rules

1. C++ code must not depend on Arduino headers, `Wire`, or Adafruit runtime classes.
2. Linux I2C access must stay behind the hardware abstraction in `include/sht4x/hardware.hpp`.
3. Attribution to the upstream Adafruit library must remain visible in the repository and in derived source files.
