# ESD Irrigation System

Firmware for an ESP32-based automated irrigation system.

## Project Layout

- `src/main.cpp` boots the firmware and calls the system lifecycle hooks.
- `src/system.cpp` and `src/system.h` hold the top-level application flow.
- `src/bed.cpp` and `src/bed.h` are the per-bed control logic.
- `src/config.h` is where hardware pins and project-wide configuration belong.

## Notes

- Target board: `esp32dev`
- Framework: Arduino
