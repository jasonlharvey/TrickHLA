/*
 * PURPOSE: (DCMotorSpeedController class)
 */
#ifndef DC_MOTOR_SPEED_CONTROLLER_HH
#define DC_MOTOR_SPEED_CONTROLLER_HH

#include "DCMotor.hh"
#include "motorSpeedController.hh"

class DCMotorSpeedController : public MotorSpeedController
{

  public:
   DCMotorSpeedController( DCMotor      &dc_motor,
                           double        gain,
                           const double &actual_speed,
                           const double &supply_voltage );

   ~DCMotorSpeedController() {}

   void   set_commanded_speed( double commanded_speed );
   double get_motor_voltage();

  private:
   double        motor_voltage;
   DCMotor      &motor;
   double        gain;
   const double &actual_speed;
   const double &supply_voltage;

   // Don't Allow the default constructor to be used.
   DCMotorSpeedController();
};

#endif
