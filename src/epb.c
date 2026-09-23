#include "epb.h"

#define EPB_APPLIED_POSITION 1000U
#define EPB_RELEASED_POSITION 0U

static void clear_outputs(EpbOutputs *outputs)
{
    outputs->motor_forward = false;
    outputs->motor_reverse = false;
    outputs->apply_active = false;
    outputs->release_active = false;
    outputs->state = EPB_RELEASED;
    outputs->fault = EPB_NO_FAULT;
}

static void enter_fault(EpbController *controller, EpbFault fault)
{
    controller->fault = fault;
    controller->state = EPB_FAULT;
    controller->motion_ticks = 0U;
}

void epb_init(EpbController *controller)
{
    controller->state = EPB_RELEASED;
    controller->fault = EPB_NO_FAULT;
    controller->target_position = EPB_RELEASED_POSITION;
    controller->apply_threshold_kph = 5U;
    controller->max_current_ma = 1800U;
    controller->timeout_ticks = 50U;
    controller->motion_ticks = 0U;
}

void epb_step(EpbController *controller,
              const EpbInputs *inputs,
              EpbOutputs *outputs)
{
    clear_outputs(outputs);

    if (!controller || !inputs || !outputs) {
        return;
    }

    outputs->state = controller->state;
    outputs->fault = controller->fault;

    if (!inputs->position_sensor_valid) {
        if (controller->state != EPB_FAULT) {
            enter_fault(controller, EPB_FAULT_INVALID_SENSOR);
        }
        outputs->state = controller->state;
        outputs->fault = controller->fault;
        return;
    }

    if (inputs->motor_current_ma > controller->max_current_ma) {
        if (controller->state == EPB_APPLYING ||
            controller->state == EPB_RELEASING) {
            enter_fault(controller, EPB_FAULT_MOTOR_OVERCURRENT);
            outputs->state = controller->state;
            outputs->fault = controller->fault;
            return;
        }
    }

    switch (controller->state) {
    case EPB_RELEASED:
        controller->motion_ticks = 0U;

        if (inputs->apply_request) {
            if (inputs->vehicle_speed_kph <= controller->apply_threshold_kph) {
                controller->target_position = EPB_APPLIED_POSITION;
                controller->state = EPB_APPLYING;
                controller->motion_ticks = 0U;
            }
        }
        break;

    case EPB_APPLYING:
        outputs->motor_forward = true;
        outputs->apply_active = true;

        if (inputs->actuator_position >= controller->target_position) {
            controller->state = EPB_APPLIED;
            controller->motion_ticks = 0U;
        } else {
            controller->motion_ticks++;
            if (controller->motion_ticks > controller->timeout_ticks) {
                enter_fault(controller, EPB_FAULT_POSITION_TIMEOUT);
            }
        }
        break;

    case EPB_APPLIED:
        controller->motion_ticks = 0U;

        if (inputs->release_request && inputs->brake_pedal_pressed) {
            controller->target_position = EPB_RELEASED_POSITION;
            controller->state = EPB_RELEASING;
            controller->motion_ticks = 0U;
        }
        break;

    case EPB_RELEASING:
        outputs->motor_reverse = true;
        outputs->release_active = true;

        if (inputs->actuator_position <= controller->target_position) {
            controller->state = EPB_RELEASED;
            controller->motion_ticks = 0U;
        } else {
            controller->motion_ticks++;
            if (controller->motion_ticks > controller->timeout_ticks) {
                enter_fault(controller, EPB_FAULT_POSITION_TIMEOUT);
            }
        }
        break;

    case EPB_FAULT:
        outputs->motor_forward = false;
        outputs->motor_reverse = false;
        break;

    default:
        enter_fault(controller, EPB_FAULT_INVALID_SENSOR);
        break;
    }

    outputs->state = controller->state;
    outputs->fault = controller->fault;
}

const char *epb_state_name(EpbState state)
{
    switch (state) {
    case EPB_RELEASED:  return "RELEASED";
    case EPB_APPLYING:  return "APPLYING";
    case EPB_APPLIED:   return "APPLIED";
    case EPB_RELEASING: return "RELEASING";
    case EPB_FAULT:     return "FAULT";
    default:            return "UNKNOWN";
    }
}

const char *epb_fault_name(EpbFault fault)
{
    switch (fault) {
    case EPB_NO_FAULT:                return "NONE";
    case EPB_FAULT_MOTOR_OVERCURRENT: return "MOTOR_OVERCURRENT";
    case EPB_FAULT_POSITION_TIMEOUT:  return "POSITION_TIMEOUT";
    case EPB_FAULT_INVALID_SENSOR:    return "INVALID_SENSOR";
    default:                          return "UNKNOWN";
    }
}
