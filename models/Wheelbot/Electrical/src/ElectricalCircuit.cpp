#include "ElectricalCircuit.hh"
#include <iostream>
#include "../../Motor/include/DCMotor.hh"
#include "../../Battery/include/DCBattery.hh"

ElectricalCircuit::ElectricalCircuit ()
: motors_current(0), motor1(0), motor2(0), battery(0) {

}

void ElectricalCircuit :: init (DCMotor* motorOne, DCMotor* motorTwo, DCBattery* battery1)
{
    motor1 = motorOne;
    motor2 = motorTwo;
    battery = battery1;
}

void ElectricalCircuit :: update ()
{
    motors_current = motor1 -> get_current_load() + motor2 -> get_current_load();
    battery->set_current(motors_current);

}
