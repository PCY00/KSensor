//tmc2209 library

#ifndef _STEPPERMOTER_H_
#define _STEPPERMOTER_H_

#include <Arduino.h>

class StepperMoter
{
  public:
    StepperMoter(int stepPin, int dirPin, int enPin);
    void moveto(int speeds, float stepperRound);

  private:
    int stepPin;
    int dirPin;
    int enPin;
};


#endif
