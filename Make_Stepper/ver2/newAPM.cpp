#include "newAPM.h"

//Bottom Moter driver Pin setting
newAPM::newAPM(int stepPin_1, int dirPin_1, int enPin_1){
  this->stepPin_1 = stepPin_1;
  this->dirPin_1 = dirPin_1;
  this->enPin_1 = enPin_1;

  pinMode(stepPin_1, OUTPUT);
  pinMode(dirPin_1, OUTPUT);
  pinMode(enPin_1, OUTPUT);

  digitalWrite(enPin_1, HIGH);
  digitalWrite(dirPin_1, HIGH);
}

//Top Moter driver Pin setting
newAPM::newAPM(int stepPin_0, int dirPin_0, int enPin_0, int stepPin_1, int dirPin_1, int enPin_1, int stepPin_2, int dirPin_2, int enPin_2, int stepPin_3, int dirPin_3, int enPin_3, int swich_1, int swich_2, int swich_3){
    this->stepPin_0 = stepPin_0;
    this->dirPin_0 = dirPin_0;
    this->enPin_0 = enPin_0;
    
    this->stepPin_1 = stepPin_1;
    this->dirPin_1 = dirPin_1;
    this->enPin_1 = enPin_1;
    
    this->stepPin_2 = stepPin_2;
    this->dirPin_2 = dirPin_2;
    this->enPin_2 = enPin_2;
    
    this->stepPin_3 = stepPin_3;
    this->dirPin_3 = dirPin_3;
    this->enPin_3 = enPin_3;
    
    this->swich_1 = swich_1;
    this->swich_2 = swich_2;
    this->swich_3 = swich_3;
    
    pinMode(stepPin_0, OUTPUT);
    pinMode(dirPin_0, OUTPUT);
    pinMode(enPin_0, OUTPUT);

    pinMode(stepPin_1, OUTPUT);
    pinMode(dirPin_1, OUTPUT);
    pinMode(enPin_1, OUTPUT);
    
    pinMode(stepPin_2, OUTPUT);
    pinMode(dirPin_2, OUTPUT);
    pinMode(enPin_2, OUTPUT);
    
    pinMode(stepPin_3, OUTPUT);
    pinMode(dirPin_3, OUTPUT);
    pinMode(enPin_3, OUTPUT);
    
    pinMode(swich_1, INPUT_PULLUP);
    pinMode(swich_2, INPUT_PULLUP);
    pinMode(swich_3, INPUT_PULLUP);
    
    digitalWrite(enPin_0, HIGH);
    digitalWrite(enPin_1, HIGH);
    digitalWrite(enPin_2, HIGH);
    digitalWrite(enPin_3, HIGH);
    
    digitalWrite(dirPin_0, LOW);
    //아래쪽 방향
    digitalWrite(dirPin_1, HIGH);
    digitalWrite(dirPin_2, HIGH);
    digitalWrite(dirPin_3, HIGH);
}

//1스텝씩 이동 speeds = 0.1 ~ 2권장
void newAPM::moveto(int stepPin, int speeds){
    //1스텝씩 이동
    digitalWrite(stepPin, HIGH);
    delay(speeds);
    digitalWrite(stepPin, LOW);
    delay(speeds);
}

//1600 = 1바퀴
void newAPM::run(int speeds, float stepperRound){
    int stepperRotations_t = (int)(abs(stepperRound*1600));

    if(stepperRound < 0){
        digitalWrite(dirPin_0, LOW);
        digitalWrite(dirPin_1, LOW);
        digitalWrite(dirPin_2, LOW);
        digitalWrite(dirPin_3, LOW);
        for(int i = 0; i < stepperRotations_t; i++){
            moveto(stepPin_0, speeds);
            moveto(stepPin_1, speeds);
            moveto(stepPin_2, speeds);
            moveto(stepPin_3, speeds);
        }
    }else{
        digitalWrite(dirPin_0, HIGH);
        digitalWrite(dirPin_1, HIGH);
        digitalWrite(dirPin_2, HIGH);
        digitalWrite(dirPin_3, HIGH);
        for(int i = 0; i < stepperRotations_t; i++){
            moveto(stepPin_0, speeds);
            moveto(stepPin_1, speeds);
            moveto(stepPin_2, speeds);
            moveto(stepPin_3, speeds);
        }
    }
}

//Top Moter driver 영점 맞추기
void newAPM::motorTopSetup(){
  while(1){
    //3개의 스위치가 눌렸을 때
    if(digitalRead(swich_1) == LOW && digitalRead(swich_2) == LOW && digitalRead(swich_3) == LOW){
      break;
    }else{
      digitalWrite(stepPin_1, HIGH);
      digitalWrite(stepPin_2, HIGH);
      digitalWrite(stepPin_3, HIGH);
      delay(1);
      digitalWrite(stepPin_1, LOW);
      digitalWrite(stepPin_2, LOW);
      digitalWrite(stepPin_3, LOW);
      delay(1);
    }
  }
}
