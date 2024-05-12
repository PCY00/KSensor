#include "misumi.h"
#include <Arduino.h>

misumi::misumi(int driverPUL, int driverDIR, long resolution) {
    this->driverPUL = driverPUL;
    this->driverDIR = driverDIR;
    this->PR = resolution;

    this->preMicros = 0;
    this->leftSteps = resolution * 2;

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
    this->leftSteps = this->PR * 2;
    setdir = !setdir;
    digitalWrite(driverDIR, setdir);
}

void misumi::move_motor_angle(bool rev){
    unsigned long currentMicros = micros();

    if(rev == false){
        if(currentMicros - this->preMicros >= this->delaySpeed){
            this->preMicros = currentMicros;
            if(this->leftSteps > 0){
                this->leftSteps--;
                move_moter(leftSteps % 2);
            }
        }
    }else {
        if(currentMicros - this->preMicros >= this->delaySpeed){
            this->preMicros = currentMicros;
            if(this->leftSteps > 0){
                this->leftSteps--;
                move_moter(leftSteps % 2);
            }else {
              revmotor();
            }
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
