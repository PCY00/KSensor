#include <Arduino.h>
#include <AccelStepper.h>
#define motorInterfaceType 1

#define BOTTOM_SPEED 100

const int dirPin_Top_1 = 4;
const int stepPin_Top_1 = 5;

const int dirPin_Top_2 = 8;
const int stepPin_Top_2 = 9;


const int dirPin_Top_3 = 12;
const int stepPin_Top_3 = 13;


const int dirPin_Bottom_1 = 2;
const int stepPin_Bottom_1 = 3;

const int dirPin_Bottom_2 = 6;
const int stepPin_Bottom_2 = 7;

const int dirPin_Bottom_3 = 10;
const int stepPin_Bottom_3 = 11;


AccelStepper stepper_Top_1(motorInterfaceType, stepPin_Top_1, dirPin_Top_1);
AccelStepper stepper_Top_2(motorInterfaceType, stepPin_Top_2, dirPin_Top_2);
AccelStepper stepper_Top_3(motorInterfaceType, stepPin_Top_3, dirPin_Top_3);


AccelStepper stepper_Bottom_1(motorInterfaceType, stepPin_Bottom_1, dirPin_Bottom_1);
AccelStepper stepper_Bottom_2(motorInterfaceType, stepPin_Bottom_2, dirPin_Bottom_2);
AccelStepper stepper_Bottom_3(motorInterfaceType, stepPin_Bottom_3, dirPin_Bottom_3);

void setup()
{
  // put your setup code here, to run once:
  
  stepper_Top_1.setMaxSpeed(500.0);
  stepper_Top_1.setAcceleration(20.0);
  stepper_Top_1.moveTo(55000);
  stepper_Top_1.setSpeed(500);

  stepper_Top_2.setMaxSpeed(500.0);
  stepper_Top_2.setAcceleration(20.0);
  stepper_Top_2.moveTo(55000);
  stepper_Top_2.setSpeed(500);

  stepper_Top_3.setMaxSpeed(500.0);
  stepper_Top_3.setAcceleration(20.0);
  stepper_Top_3.moveTo(55000);
  stepper_Top_3.setSpeed(500);
  

  stepper_Bottom_1.setMaxSpeed(BOTTOM_SPEED);
  stepper_Bottom_1.setAcceleration(10.0);
  stepper_Bottom_1.moveTo(5000);
  stepper_Bottom_1.setSpeed(BOTTOM_SPEED);
  
  stepper_Bottom_2.setMaxSpeed(BOTTOM_SPEED);
  stepper_Bottom_2.setAcceleration(10.0);
  stepper_Bottom_2.moveTo(5000);
  stepper_Bottom_2.setSpeed(BOTTOM_SPEED);

  stepper_Bottom_3.setMaxSpeed(BOTTOM_SPEED);
  stepper_Bottom_3.setAcceleration(10.0);
  stepper_Bottom_3.moveTo(5000);
  stepper_Bottom_3.setSpeed(BOTTOM_SPEED);
}

void loop()
{
  // put your main code here, to run repeatedly:
  
  if (stepper_Top_1.distanceToGo() == 0)
  {
    stepper_Top_1.moveTo(-stepper_Top_1.currentPosition());
  }
  if (stepper_Top_2.distanceToGo() == 0)
  {
    stepper_Top_2.moveTo(-stepper_Top_2.currentPosition());
  }
  if (stepper_Top_3.distanceToGo() == 0)
  {
    stepper_Top_3.moveTo(-stepper_Top_3.currentPosition());
  }
  
  
  
  if (stepper_Bottom_1.distanceToGo() == 0)
  {
    stepper_Bottom_1.moveTo(-stepper_Bottom_1.currentPosition());
  }
  if (stepper_Bottom_2.distanceToGo() == 0)
  {
    stepper_Bottom_2.moveTo(-stepper_Bottom_2.currentPosition());
  }
  if (stepper_Bottom_3.distanceToGo() == 0)
  {
    stepper_Bottom_3.moveTo(-stepper_Bottom_3.currentPosition());
  }

  stepper_Top_1.run();
  stepper_Top_2.run();
  stepper_Top_3.run();
  
  stepper_Bottom_1.run();
  stepper_Bottom_2.run();
  stepper_Bottom_3.run();
}
