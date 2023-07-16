#include "APM.h"
 
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

//모터 속도
#define speeds 1
/*
 * 여기서 모터 한바퀴는 1600스텝이다.
 * bottom일 경우 APM 한바퀴가 몇인지 알아야함
 * top일 경우 초기화후 위로 올라가는게 몇 까지 올라가야하는지 알아야함
 * 
 * 위를 토대로 rounds를 설정해줘야함
 * 기본값으로 4800이 한바퀴라고 설정을 해둠 (조정필요)
*/
#define rounds 1

StepperMoter bottom(step_B, dir_B, en_B);
StepperMoter top_1(step_T1, dir_T1, en_T1, swich_1);
StepperMoter top_2(step_T2, dir_T2, en_T2, swich_2);
StepperMoter top_3(step_T3, dir_T3, en_T3, swich_3);

void setup(){
  top_1.motorTopSetup();
  top_2.motorTopSetup();
  top_3.motorTopSetup();

  bottom.setting(speeds, rounds);
  top_1.setting(speeds, rounds);
  top_2.setting(speeds, rounds);
  top_3.setting(speeds, rounds);
  
  delay(500);
}

void loop(){
  //bottom control
  bottom.run();
  
  //top control
  top_1.run();
  top_2.run();
  top_3.run();
}
