#include "stepperMoter.h"
 
#define en_B 2
#define step_B 3
#define dir_B 4

#define speeds 1

StepperMoter bottom(step_B, dir_B, en_B);

void setup(){
  
}

void loop(){
  bottom.moveto(speeds, 1.0);
  delay(1000);
  bottom.moveto(speeds, -1.0);
  delay(1000);
}
