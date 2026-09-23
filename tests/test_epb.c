#include <assert.h>
#include <stdio.h>
#include "epb.h"

static EpbInputs base_inputs(void)
{
    EpbInputs in = {
        .apply_request = false,
        .release_request = false,
        .brake_pedal_pressed = false,
        .vehicle_speed_kph = 0U,
        .actuator_position = 0U,
        .motor_current_ma = 500U,
        .position_sensor_valid = true
    };
    return in;
}

static void test_initial_state(void)
{
    EpbController c;
    EpbOutputs out;
    EpbInputs in = base_inputs();

    epb_init(&c);
    epb_step(&c, &in, &out);

    assert(c.state == EPB_RELEASED);
    assert(c.fault == EPB_NO_FAULT);
}

static void test_apply_stationary(void)
{
    EpbController c;
    EpbOutputs out;
    EpbInputs in = base_inputs();

    epb_init(&c);
    in.apply_request = true;

    epb_step(&c, &in, &out);
    assert(c.state == EPB_APPLYING);
}

static void test_apply_blocked_when_moving(void)
{
    EpbController c;
    EpbOutputs out;
    EpbInputs in = base_inputs();

    epb_init(&c);
    in.apply_request = true;
    in.vehicle_speed_kph = 25U;

    epb_step(&c, &in, &out);
    assert(c.state == EPB_RELEASED);
}

static void test_release_requires_brake_pedal(void)
{
    EpbController c;
    EpbOutputs out;
    EpbInputs in = base_inputs();

    epb_init(&c);
    c.state = EPB_APPLIED;
    in.release_request = true;
    in.brake_pedal_pressed = false;
    in.actuator_position = 1000U;

    epb_step(&c, &in, &out);
    assert(c.state == EPB_APPLIED);
}

static void test_release_with_brake_pedal(void)
{
    EpbController c;
    EpbOutputs out;
    EpbInputs in = base_inputs();

    epb_init(&c);
    c.state = EPB_APPLIED;
    in.release_request = true;
    in.brake_pedal_pressed = true;
    in.actuator_position = 1000U;

    epb_step(&c, &in, &out);
    assert(c.state == EPB_RELEASING);
}

static void test_overcurrent_fault(void)
{
    EpbController c;
    EpbOutputs out;
    EpbInputs in = base_inputs();

    epb_init(&c);
    c.state = EPB_APPLYING;
    in.actuator_position = 0U;
    in.motor_current_ma = 2500U;

    epb_step(&c, &in, &out);
    assert(c.state == EPB_FAULT);
    assert(c.fault == EPB_FAULT_MOTOR_OVERCURRENT);
}

static void test_invalid_sensor_fault(void)
{
    EpbController c;
    EpbOutputs out;
    EpbInputs in = base_inputs();

    epb_init(&c);
    in.position_sensor_valid = false;

    epb_step(&c, &in, &out);
    assert(c.state == EPB_FAULT);
    assert(c.fault == EPB_FAULT_INVALID_SENSOR);
}

int main(void)
{
    test_initial_state();
    test_apply_stationary();
    test_apply_blocked_when_moving();
    test_release_requires_brake_pedal();
    test_release_with_brake_pedal();
    test_overcurrent_fault();
    test_invalid_sensor_fault();

    puts("All EPB simulator tests passed.");
    return 0;
}
