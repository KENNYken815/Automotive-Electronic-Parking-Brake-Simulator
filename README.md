# Automotive Electronic Parking Brake (EPB) Simulator

A presentation-ready embedded-systems simulation of an **Automotive Electronic Parking Brake (EPB)** controller.

The project models a simplified EPB ECU that accepts driver commands, monitors vehicle speed and brake-pedal state, controls a parking-brake actuator, and detects common simulated faults.

> **Educational simulator:** this repository demonstrates embedded control logic and diagnostics. It is **not** suitable for direct deployment in a real vehicle.

## Project highlights

- EPB apply and release control
- Vehicle-speed interlock
- Brake-pedal interlock
- Actuator position feedback
- Current/overload supervision
- Fault detection and safe-state handling
- Controller state machine
- Event/status reporting through a serial console
- Hardware-independent C implementation suitable for porting to an MCU
- Unit-test style simulation scenarios
- Automotive-oriented architecture and documentation

## Functional concept

```text
 Driver Switch ─────┐
                    │
 Brake Pedal ───────┤
 Vehicle Speed ─────┤
 Actuator Feedback ─┤
 Motor Current ─────┤
                    ▼
             +---------------+
             |    EPB ECU     |
             |---------------|
             | Input Manager  |
             | State Machine  |
             | Safety Logic   |
             | Diagnostics    |
             +-------+-------+
                     |
              H-Bridge / Motor
                     |
                     ▼
              Parking Brake
               Actuator
                     │
             Position Feedback
                     └──────────► ECU
```

## Control states

| State | Meaning |
|---|---|
| `RELEASED` | Parking brake is fully released |
| `APPLYING` | Actuator is moving toward the applied position |
| `APPLIED` | Parking brake is fully applied |
| `RELEASING` | Actuator is moving toward the released position |
| `FAULT` | A critical fault has been detected; motor output is disabled |

## Safety/interlock behavior

### Apply request
The simulator accepts an apply request when the control conditions are valid. If the vehicle is moving above the configurable apply threshold, the command is rejected in normal mode.

### Release request
Release is permitted only when the brake pedal is pressed. This models a common driver-intent interlock used in simplified EPB concepts.

### Actuator protection
During movement, the ECU watches simulated motor current and position. Excessive current causes a fault condition and disables the actuator command.

### Fault handling
A detected fault transitions the controller to `FAULT`, clears the motor command, and exposes a diagnostic code for the console.

## Repository structure

```text
.
├── README.md
├── LICENSE
├── .gitignore
├── include/
│   └── epb.h
├── src/
│   ├── epb.c
│   └── main.c
├── tests/
│   └── test_epb.c
└── docs/
    ├── architecture.md
    └── presentation.md
```

## Building

The simulator is written in standard C.

### GCC / Clang

```bash
gcc -std=c11 -Wall -Wextra -pedantic src/epb.c src/main.c -Iinclude -o epb_sim
./epb_sim
```

### Run the test scenarios

```bash
gcc -std=c11 -Wall -Wextra -pedantic src/epb.c tests/test_epb.c -Iinclude -o test_epb
./test_epb
```

## Example demonstration flow

The included `main.c` runs a scripted demonstration:

1. ECU starts in `RELEASED`.
2. Driver requests EPB apply while stationary.
3. Actuator travels to the applied position.
4. Driver presses the brake pedal and requests release.
5. Actuator returns to the released position.
6. A simulated over-current condition is injected.
7. ECU records the fault and enters `FAULT`.

Example console output:

```text
[EPB] ECU initialized
[EPB] State: RELEASED
[EPB] Apply request accepted
[EPB] State: APPLYING
[EPB] Actuator reached APPLY position
[EPB] State: APPLIED
[EPB] Brake pedal: PRESSED
[EPB] Release request accepted
[EPB] State: RELEASING
[EPB] Actuator reached RELEASE position
[EPB] State: RELEASED
[EPB] Injecting over-current fault...
[EPB] FAULT: MOTOR_OVERCURRENT
```

## Suggested hardware expansion

The software architecture can be mapped to a real embedded prototype:

- STM32 / NXP S32K / Infineon AURIX-class MCU
- CAN or CAN-FD communication
- H-bridge motor driver
- DC geared motor or linear actuator
- Hall/encoder position sensor
- Current shunt + ADC
- Brake-pedal switch input
- Vehicle-speed input from CAN
- Automotive power conditioning

For a physical demonstrator, the actuator should be mechanically isolated and should **not** be connected to a road-going vehicle braking system.

## Learning outcomes

This project demonstrates:

- Embedded C state-machine design
- Sensor/input validation
- Actuator control
- Interlocks and fail-safe behavior
- Diagnostic fault codes
- Deterministic control flow
- Hardware abstraction concepts
- Test-driven simulation scenarios
- Automotive ECU architecture

## Portfolio value

This project is suitable as an **embedded systems / automotive electronics** portfolio project because it combines control logic, fault handling, simulation, and automotive-oriented software structure.

### Resume description

> **Automotive Electronic Parking Brake (EPB) Simulator** — Developed a C-based EPB ECU simulator implementing apply/release state-machine control, brake-pedal and vehicle-speed interlocks, actuator position feedback, over-current fault detection, and diagnostic reporting, with standalone simulation tests.

## Presentation talking points

See [docs/presentation.md](docs/presentation.md) for a ready-to-use presentation flow.

See [docs/architecture.md](docs/architecture.md) for the software architecture and state transitions.

## Author

**Kenny (Palem Raj Narayan)**  
Electronics & Communication Engineering

---

## Safety note

Real EPB systems are safety-critical automotive systems and require substantially more engineering than this educational model, including redundant sensing/actuation, timing guarantees, diagnostics, communication safety, automotive qualification, and validation against applicable standards and OEM requirements.
