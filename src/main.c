#include <stdio.h>
#include "epb.h"

static void print_status(const EpbController *controller,
                         const EpbOutputs *outputs)
{
    printf("[EPB] State=%s | Fault=%s | FWD=%s | REV=%s\n",
           epb_state_name(outputs->state),
           epb_fault_name(outputs->fault),
           outputs->motor_forward ? "ON" : "OFF",
           outputs->motor_reverse ? "ON" : "OFF");
}

static void run_apply_sequence(EpbController *controller)
{
    EpbInputs in = {
        .apply_request = true,
        .release_request = false,
        .brake_pedal_pressed = false,
        .vehicle_speed_kph = 0U,
        .actuator_position = 0U,
        .motor_current_ma = 900U,
        .position_sensor_valid = true
    };
    EpbOutputs out;

    printf("\n=== APPLY DEMONSTRATION ===\n");

    epb_step(controller, &in, &out);
    print_status(controller, &out);

    in.apply_request = false;

    while (controller->state == EPB_APPLYING) {
        if (in.actuator_position < 1000U) {
            in.actuator_position += 250U;
        }
        epb_step(controller, &in, &out);
        print_status(controller, &out);
    }
}

static void run_release_sequence(EpbController *controller)
{
    EpbInputs in = {
        .apply_request = false,
        .release_request = true,
        .brake_pedal_pressed = true,
        .vehicle_speed_kph = 0U,
        .actuator_position = 1000U,
        .motor_current_ma = 900U,
        .position_sensor_valid = true
    };
    EpbOutputs out;

    printf("\n=== RELEASE DEMONSTRATION ===\n");

    epb_step(controller, &in, &out);
    print_status(controller, &out);

    in.release_request = false;

    while (controller->state == EPB_RELEASING) {
        if (in.actuator_position >= 250U) {
            in.actuator_position -= 250U;
        } else {
            in.actuator_position = 0U;
        }
        epb_step(controller, &in, &out);
        print_status(controller, &out);
    }
}

static void run_fault_sequence(EpbController *controller)
{
    EpbInputs in = {
        .apply_request = true,
        .release_request = false,
        .brake_pedal_pressed = false,
        .vehicle_speed_kph = 0U,
        .actuator_position = 0U,
        .motor_current_ma = 2500U,
        .position_sensor_valid = true
    };
    EpbOutputs out;

    printf("\n=== FAULT DEMONSTRATION ===\n");

    epb_step(controller, &in, &out);
    print_status(controller, &out);

    /* The first step starts APPLYING. Inject current during actuator motion. */
    in.apply_request = false;
    epb_step(controller, &in, &out);
    print_status(controller, &out);
}

int main(void)
{
    EpbController controller;

    printf("Automotive Electronic Parking Brake (EPB) Simulator\n");
    printf("Educational embedded control demonstration\n");

    epb_init(&controller);

    {
        EpbInputs boot = {
            .apply_request = false,
            .release_request = false,
            .brake_pedal_pressed = false,
            .vehicle_speed_kph = 0U,
            .actuator_position = 0U,
            .motor_current_ma = 0U,
            .position_sensor_valid = true
        };
        EpbOutputs out;

        epb_step(&controller, &boot, &out);
        print_status(&controller, &out);
    }

    run_apply_sequence(&controller);
    run_release_sequence(&controller);

    /*
     * Re-start from RELEASED and demonstrate an over-current event.
     */
    epb_init(&controller);
    run_fault_sequence(&controller);

    printf("\nSimulation complete.\n");
    return 0;
}
