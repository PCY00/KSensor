#include "misumi.h"
#include <Arduino.h>

misumi::misumi(int driverPUL, int driverDIR, long resolution) {
    this->driverPUL = driverPUL;
    this->driverDIR = driverDIR;
    this->PR = resolution;

    this->preMicros = 0;
    this->leftSteps = 0;

    this->setdir = HIGH;

    pinMode(driverPUL, OUTPUT);
    pinMode(driverDIR, OUTPUT);

    digitalWrite(driverDIR, setdir);
}


void misumi::setSpeed(int delaySpeed) {
//101 ~ 16000
    this->delaySpeed = delaySpeed;
}

void misumi::revmotor() {
    setdir = !setdir;
    digitalWrite(driverDIR, setdir);
}

void misumi::set_angle(long angle){
  this->leftSteps = (this->PR / (360 / angle))*2;
}

void misumi::move_motor_angle(){
    unsigned long currentMicros = micros();

    if(currentMicros - this->preMicros >= this->delaySpeed){
        this->preMicros = currentMicros;
        if(this->leftSteps > 0){
            this->leftSteps--;
            move_moter(leftSteps % 2);
        }
    }
}

void misumi::move_moter(long steps){
    switch(steps){
        case 0:
            digitalWrite(driverPUL, HIGH);
            break;
        case 1:
            digitalWrite(driverPUL, LOW);
            break;
    }
}
/*
void misumi::test_function(int PR) {
    for (int i = 0; i < PR; i++) {
        digitalWrite(driverPUL, HIGH);
        delayMicroseconds(delaySpeed);
        digitalWrite(driverPUL, LOW);
        delayMicroseconds(delaySpeed);
    }
}*/
