#include "misumi.h"
#include <Arduino.h>

misumi::misumi(int driverPUL, int driverDIR, long resolution, int switch_0, int switch_30, int switch_60, int switch_90) {
    this->driverPUL = driverPUL;
    this->driverDIR = driverDIR;
    this->PR = resolution;

    this->preMicros = 0;
    this->leftSteps = 0;

    this->setdir = HIGH;
    this->reset_check = false;

    this->switch_0 = switch_0;
    this->switch_30 = switch_30;
    this->switch_60 = switch_60;
    this->switch_90 = switch_90;

    pinMode(switch_0, INPUT_PULLUP);
    pinMode(switch_30, INPUT_PULLUP);
    pinMode(switch_60, INPUT_PULLUP);
    pinMode(switch_90, INPUT_PULLUP);

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
    //this->leftSteps = (this->PR / (360 / angle))*2*6.667;
    this->leftSteps = angle * this->PR;
}

void misumi::switch_set(int angle){
  this->angle = angle;
}

int misumi::switch_checking(){

    switch(this->angle){
      case 30:
        if(this->reset_check == false){
          reset();
        }
       if(!digitalRead(this->switch_30)){
          Serial.println("30");
          return 1;
        }
        break;
      case 60:
        if(this->reset_check == false){
          reset();
        }
       if(!digitalRead(this->switch_60)){
          Serial.println("60");
          return 1;
        }
        break;
      case 90:
        if(this->reset_check == false){
          reset();
        }
       if(!digitalRead(this->switch_90)){
          Serial.println("90");
          return 1;
        }
        break;
      default:
        //Serial.println("switch_error");
        return 0;
        break;
        
    }
}

void misumi::move_motor_angle(){
    unsigned long currentMicros = micros();

    if(currentMicros - this->preMicros >= this->delaySpeed){
        this->preMicros = currentMicros;
        if(this->leftSteps > 0){
            this->leftSteps--;
            move_moter(leftSteps % 2);
        }
        if(switch_checking() == 1){
            this->leftSteps = 0;
            this->angle = 713;
            this->reset_check = false;
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

void misumi::reset(){
  while(1){
    if(!digitalRead(this->switch_0)){
      Serial.println("0");
      this->reset_check = true;
      break;
    }
    digitalWrite(driverPUL, HIGH);
    delay(10);
    digitalWrite(driverPUL, LOW);
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
