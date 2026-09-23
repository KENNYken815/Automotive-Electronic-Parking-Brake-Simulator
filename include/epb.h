#ifndef EPB_H
#define EPB_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    EPB_RELEASED = 0,
    EPB_APPLYING,
    EPB_APPLIED,
    EPB_RELEASING,
    EPB_FAULT
} EpbState;

typedef enum {
    EPB_NO_FAULT = 0,
    EPB_FAULT_MOTOR_OVERCURRENT,
    EPB_FAULT_POSITION_TIMEOUT,
    EPB_FAULT_INVALID_SENSOR
} EpbFault;

typedef struct {
    bool apply_request;
    bool release_request;
    bool brake_pedal_pressed;
    uint16_t vehicle_speed_kph;
    uint16_t actuator_position;
    uint16_t motor_current_ma;
    bool position_sensor_valid;
} EpbInputs;

typedef struct {
    bool motor_forward;
    bool motor_reverse;
    bool apply_active;
    bool release_active;
    EpbState state;
    EpbFault fault;
} EpbOutputs;

typedef struct {
    EpbState state;
    EpbFault fault;
    uint16_t target_position;
    uint16_t apply_threshold_kph;
    uint16_t max_current_ma;
    uint16_t timeout_ticks;
    uint16_t motion_ticks;
} EpbController;

void epb_init(EpbController *controller);
void epb_step(EpbController *controller,
              const EpbInputs *inputs,
              EpbOutputs *outputs);

const char *epb_state_name(EpbState state);
const char *epb_fault_name(EpbFault fault);

#endif
