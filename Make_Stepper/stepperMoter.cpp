#include "stepperMoter.h"

StepperMoter::StepperMoter(int stepPin, int dirPin, int enPin){
  this->stepPin = stepPin;
  this->dirPin = dirPin;
  this->enPin = enPin;

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);

  digitalWrite(enPin, HIGH);
  digitalWrite(dirPin, HIGH);
}

//1600step = 1round, speed = 0.1 ~ 2 (권장 1)
//음수일 때 와 양수일 때로 나누어 모터 방향 제어
void StepperMoter::moveto(int speeds, float stepperRound){
  int stepperRotations_t = (int)(abs(stepperRound*1600));
  if(stepperRound < 0){
    digitalWrite(dirPin, LOW);
    for(int i = 0; i < stepperRotations_t; i++){
      digitalWrite(stepPin, HIGH);
      delay(speeds);
      digitalWrite(stepPin, LOW);
      delay(speeds);
    }
  }else{
    digitalWrite(dirPin, HIGH);
    for(int i = 0; i < stepperRotations_t; i++){
      digitalWrite(stepPin, HIGH);
      delay(speeds);
      digitalWrite(stepPin, LOW);
      delay(speeds);
    }
  }
}
