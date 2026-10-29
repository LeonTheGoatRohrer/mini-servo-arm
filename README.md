# Mini Servo Arm

A two-axis ESP32-C3 robot arm controlled from a local browser over its own Wi-Fi access point.

## Implemented features

- Browser joysticks for base and shoulder velocity.
- Explicit motor activation, stop/hold, centre and PWM-off commands.
- 350 ms command timeout.
- Parametric KCL CAD assembly and individual printable parts.

## Technology

PlatformIO, Arduino C++, ESP32-C3, HTML/JavaScript; KCL parametric CAD.

## Installation and local use

1. Copy `firmware/src/local_config.example.h` to `firmware/src/local_config.h` and choose a unique access-point password.
2. Open `firmware/` in PlatformIO and build with `pio run`.
3. Select the actual connected board port, then upload using PlatformIO.
4. Join `MiniServoArm-C3` and open `http://192.168.42.1`. Activate the motors deliberately.

## Configuration and requirements

ESP32-C3, two servo motors (HS-325HB used in the design), suitable external 4.8–6 V servo supply and common ground. Do not power the servos from a GPIO. Local secrets are ignored.

## Development status and limitations

Hardware prototype. Base PWM range: 1450–1550 µs; shoulder: 1500–2100 µs. Actual angles are not sensed or fully calibrated. A nominal 90° shoulder position is not reachable with the recorded mechanical zero. Historical bench observations in the German notes are not new acceptance tests. Firmware compilation and physical acceptance are reported separately in `PUBLICATION_CHECKS.md`.

## Contribution and attribution

Leon Rohrer: personal hardware/firmware project. Retain the existing component specifications and any third-party notices in the source.

## Wiring and CAD

| Signal | ESP32-C3 |
|---|---|
| Base servo PWM | GPIO6 |
| Shoulder servo PWM | GPIO7 |
| Servo ground | Common GND |

Firmware entry point: `firmware/src/main.cpp`; browser controls: `firmware/src/control_page.h`. CAD lives in `Prototyp/p1/`; open `main.kcl` for the assembly and `printLayout.kcl` for the print layout. Read `NOTES.md` before printing. The redundant ZIP and cache files are omitted.

## Publication notes

This portfolio copy includes project source and relevant documentation. Local credentials, private datasets, dependencies and generated build files are excluded. No deployment or paid service is configured by this publication. Existing copyright and license notices remain applicable; this publication does not introduce a new license.
