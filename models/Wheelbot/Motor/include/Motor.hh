#ifndef MOTOR
#define MOTOR
/********************************* TRICK HEADER *******************************
PURPOSE: ()
LIBRARY DEPENDENCY:
    ((Motor.o))
*******************************************************************************/
#include "PWM.hh"
class Motor
{
  public:

    Motor ();

    virtual void update (const PWM& pulse_width) = 0;
    virtual ~Motor();

    virtual double get_actual_speed ();
};
#endif
