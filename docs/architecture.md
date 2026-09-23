# EPB Simulator Architecture

## 1. ECU responsibilities

The simulated EPB ECU performs four primary functions:

1. Read driver and sensor inputs.
2. Decide whether an EPB transition is permitted.
3. Command the actuator.
4. Detect faults and force a safe output state.

## 2. Input signals

- `apply_request`: driver requests parking-brake application.
- `release_request`: driver requests release.
- `brake_pedal_pressed`: interlock input for release.
- `vehicle_speed_kph`: movement interlock.
- `actuator_position`: normalized actuator position, 0–1000.
- `motor_current_ma`: simulated motor current.
- `position_sensor_valid`: diagnostic validity flag.

## 3. Output signals

- `motor_forward`: drive actuator toward applied position.
- `motor_reverse`: drive actuator toward released position.
- `apply_active`
- `release_active`
- Current controller state.
- Current diagnostic fault.

## 4. State transition model

```text
                  apply request
 RELEASED ----------------------------> APPLYING
    ^                                     |
    |                                     | target reached
    |                                     v
    |                                   APPLIED
    |                                     |
    |                    brake pedal +    |
    |                    release request  |
    |                                     v
    +<-------------------------------- RELEASING

 Any critical sensor/current/timeout fault
                 |
                 v
               FAULT
```

## 5. Fault strategy

### Motor over-current
When motor current exceeds the configured limit during motion, the ECU enters `FAULT` and removes both motor commands.

### Position timeout
If the actuator remains in a moving state beyond the configured motion time, the ECU enters `FAULT`.

### Invalid position sensor
If the position sensor is reported invalid, the ECU enters `FAULT` and inhibits motor operation.

## 6. Porting to a microcontroller

The current code keeps the control algorithm independent from hardware. A hardware implementation can wrap:

- GPIO driver for switches
- ADC driver for current sensing
- Timer/encoder driver for position
- CAN driver for vehicle speed
- PWM/H-bridge driver for the motor

A periodic scheduler can call `epb_step()` at a fixed control rate.

## 7. Extension roadmap

Recommended next steps:

- Add CAN-frame abstraction
- Add debouncing and plausibility checks
- Add persistent DTC storage
- Add watchdog supervision
- Add dual-channel input validation
- Add HIL test interface
- Add UDS diagnostic services
- Add CRC-protected command frames
- Port to STM32 or NXP automotive MCU
