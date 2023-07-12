#include "newAPM.h"

#define en_B 2
#define step_B 3
#define dir_B 4

#define en_T1 5
#define step_T1 6
#define dir_T1 7

#define en_T2 8
#define step_T2 9
#define dir_T2 10

#define en_T3 11
#define step_T3 12
#define dir_T3 13

#define swich_1 14
#define swich_2 15
#define swich_3 16

#define speeds 1
#define stepperRound 1.5

newAPM APM(step_B, dir_B, en_B, step_T1, dir_T1, en_T1, step_T2, dir_T2, en_T2, step_T3, dir_T3, en_T3, swich_1, swich_2, swich_3);

void setup(){
  APM.motorTopSetup();
  delay(1000);
}

void loop(){
  APM.run(speeds,stepperRound);
}
