# Presentation Flow

## Slide 1 — Title

**Automotive Electronic Parking Brake (EPB) Simulator**

Embedded C simulation of an automotive parking-brake ECU.

## Slide 2 — Problem Statement

Conventional mechanical handbrakes require direct mechanical actuation. An EPB replaces that interface with an electronically controlled actuator while adding software-controlled interlocks and diagnostics.

This project builds a simplified ECU software model to demonstrate those embedded concepts.

## Slide 3 — Objectives

- Implement EPB apply/release logic.
- Prevent unsafe command combinations.
- Monitor actuator feedback.
- Detect over-current, timeout, and sensor faults.
- Keep the design hardware-independent for MCU porting.

## Slide 4 — System Architecture

Driver inputs + vehicle/sensor data → EPB ECU → motor driver → actuator → position feedback.

## Slide 5 — State Machine

Explain:

- RELEASED
- APPLYING
- APPLIED
- RELEASING
- FAULT

Emphasize that the actuator is controlled through deterministic state transitions rather than scattered if/else commands.

## Slide 6 — Safety Logic

Show two important interlocks:

**Apply:** permitted only at/below the configured low-speed threshold.

**Release:** requires brake-pedal confirmation.

Then explain actuator-current and sensor-validity checks.

## Slide 7 — Fault Handling

Three simulated faults:

- MOTOR_OVERCURRENT
- POSITION_TIMEOUT
- INVALID_SENSOR

Every critical fault disables both motor directions and moves the ECU into FAULT.

## Slide 8 — Software Structure

```text
include/epb.h     -> data types and API
src/epb.c         -> controller/state machine
src/main.c        -> demonstration
tests/test_epb.c  -> functional tests
docs/             -> architecture and presentation notes
```

## Slide 9 — Demo

Run:

```bash
gcc -std=c11 -Wall -Wextra -pedantic src/epb.c src/main.c -Iinclude -o epb_sim
./epb_sim
```

Then show apply, release, and over-current fault sequence.

## Slide 10 — Future Scope

- CAN/CAN-FD integration
- UDS diagnostics
- STM32/NXP target implementation
- Real actuator driver
- HIL testing
- More comprehensive sensor plausibility
- Safety-oriented software development process

## Final statement

This project demonstrates how automotive functionality can be structured as a deterministic embedded controller with explicit states, interlocks, actuator supervision, and fault handling.
