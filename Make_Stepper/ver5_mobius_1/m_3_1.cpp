#include "m_3_1.h"
#include <Arduino.h>

//top, bottom 모터를 위한 클래스
m_3_1::m_3_1(int enPin, int stepPin, int dirPin, int swich){
    this->enPin = enPin;
    this->stepPin = stepPin;
    this->dirPin = dirPin;
    this->swich = swich;

    this->preMillis = 0;

    this->diraction = true;


    pinMode(enPin, OUTPUT);
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(swich, INPUT_PULLUP);

    digitalWrite(enPin, LOW);
    digitalWrite(dirPin, LOW);
}

//Top bottom motor setup
void m_3_1::MotorSetup(){
    while(1){
        if(digitalRead(swich) == LOW){
            setDiraction();
            break;
        }
        digitalWrite(stepPin, HIGH);
        delay(1);
        digitalWrite(stepPin, LOW);
        delay(1);
    }
}

//millis() 단위로 입력 받는 함수
void m_3_1::setspeed(int inputSpeed){
    this->speeds = inputSpeed;
    Serial.println(this->speeds);
}

//round를 설정하는 함수
void m_3_1::setRound(long inputRound){
    this->rounds = inputRound;

    this->leftSteps = abs(inputRound) * 2;
}

//diraction이 true면 HIGH, false면 LOW
void m_3_1::setDiraction(){
    this->diraction = !this->diraction;
    if(this->diraction){
        digitalWrite(dirPin, HIGH);
    }else{
        digitalWrite(dirPin, LOW);
    }
}


//마이크로초 시간 체크해서 움직이는 함수, 한 라운드, 즉 설정된 스텝이 끝나면 반대 방향으로 돌아감
//20step당 1도 bottom만 해당 (leftStep은 40step당 1도)
void m_3_1::moveto(){
    //unsigned long currentMillis = millis();
    unsigned long currentMillis = micros();

    if(currentMillis - this->preMillis >= this->speeds){
        this->preMillis = currentMillis;

        if(this->leftSteps > 0){
            this->leftSteps--;
            moveStep(leftSteps % 2);
        }else if(this->leftSteps == 0){
            this->leftSteps = abs(this->rounds) * 2;
            //currentMillis = 0;
            //this->preMillis = currentMillis;
            setDiraction();
        }
    }
}

//실질적으로 움직이는 함수 0.5스텝
void m_3_1::moveStep(long steps){
    switch(steps){
        case 0:
            digitalWrite(stepPin, HIGH);
            break;
        case 1:
            digitalWrite(stepPin, LOW);
            break;
    }
}
