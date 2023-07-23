#include "APM.h"

int APM::step = 0;

APM::APM(int& stepPin, int& dirPin, int& enPin){
    this->stepPin = stepPin;
    this->dirPin = dirPin;
    this->enPin = enPin;

    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(enPin, OUTPUT);

    digitalWrite(enPin, HIGH);
    digitalWrite(dirPin, LOW);
}

APM::APM(int& stepPin, int& dirPin, int& enPin, int& swich){
    this->stepPin = stepPin;
    this->dirPin = dirPin;
    this->enPin = enPin;

    this->swich = swich;

    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(enPin, OUTPUT);

    pinMode(swich, INPUT_PULLUP);

    digitalWrite(enPin, HIGH);
    digitalWrite(dirPin, HIGH);
}

void APM::motorTopSetup(){
    while(1){
        if(digitalRead(swich) == LOW){
            break;
        }else{
            digitalWrite(stepPin, HIGH);
            delay(1);
            digitalWrite(stepPin, LOW);
            delay(1);
        }
    }
}

void APM::setting(int& speeds, int& rounds){
    this->speeds = speeds;
    //1600 = 모터 한바퀴, APM 1회전 = 1600 x 4 = 6400
    this->rounds = rounds;

    rounds = rounds * 6400;
}

void APM::run(){
    digitalWrite(stepPin, HIGH);
    delay(speeds);
    digitalWrite(stepPin, LOW);
    delay(speeds);

    step++;

    if(step == rounds){
        if(digitalRead(dirPin) == LOW){
            digitalWrite(dirPin, HIGH);
            step = 0;
        }else{
            digitalWrite(dirPin, LOW);
            step = 0;
        }
    }
}
