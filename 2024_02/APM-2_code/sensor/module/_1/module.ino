#include <PMS.h>                //pms7003

#define Speed_1 40
#define Speed_2 55
#define Speed_3 70

PMS pms1(Serial3);
PMS::DATA data1;

PMS pms2(Serial2);
PMS::DATA data2;

PMS pms3(Serial1);
PMS::DATA data3;

uint16_t d1, d2, d3;
const int fanPin1 = 8;
const int fanPin2 = 9;
const int fanPin3 = 10;

const int maxSpeed = 255; // 최대 속도 (0-255)

int fanSpeed1 = 0; // 현재 팬 속도
int fanSpeed2 = 0; // 현재 팬 속도
int fanSpeed3 = 0; // 현재 팬 속도

String con_data = "0,0,0";

bool startReceived = false;

void setup() {
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  Serial.begin(9600);
  delay(1000);

  pinMode(fanPin1, OUTPUT);
  pinMode(fanPin2, OUTPUT);
  pinMode(fanPin3, OUTPUT);
  
  analogWrite(fanPin1, fanSpeed1);
  analogWrite(fanPin2, fanSpeed2);
  analogWrite(fanPin3, fanSpeed3);
  
  pms1.passiveMode();
  pms2.passiveMode();
  pms3.passiveMode();

  
}

//ex) M1_P1_70

void loop() {
  //젯슨에서 값 받는거 대기
  while (!startReceived) {
  // start 신호 받기
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      if (input.startsWith("M1_P")) {
        //컨테이너 안 모듈 넘버
        if(input.substring(4,5).toInt() == 1){
          fanSpeed1 = input.substring(6).toInt();
          switch(fanSpeed1){
            case Speed_1:
              analogWrite(fanPin1, Speed_1);
              break;
            case Speed_2:
              analogWrite(fanPin1, Speed_2);
              break;
            case Speed_3:
              analogWrite(fanPin1, Speed_3);
              break;
            default:
              //Serial.println("error");
              break;
          }
        }else if(input.substring(4,5).toInt() == 2){
          fanSpeed2 = input.substring(6).toInt();
          switch(fanSpeed2){
            case Speed_1:
              analogWrite(fanPin2, Speed_1);
              break;
            case Speed_2:
              analogWrite(fanPin2, Speed_2);
              break;
            case Speed_3:
              analogWrite(fanPin2, Speed_3);
              break;
            default:
              //Serial.println("error");
              break;
          }
        }else if(input.substring(4,5).toInt() == 3){
          fanSpeed3 = input.substring(6).toInt();
          switch(fanSpeed3){
            case Speed_1:
              analogWrite(fanPin3, Speed_1);
              break;
            case Speed_2:
              analogWrite(fanPin3, Speed_2);
              break;
            case Speed_3:
              analogWrite(fanPin3, Speed_3);
              break;
            default:
              //Serial.println("error");
              break;
          } 
        }else{
          //Serial.println("module number error");
        }
      }else if(input == "start") {
        startReceived = true;
      }else{
        //Serial.println("error");
      }
    }
    con_data = String(fanSpeed1)+ "," + String(fanSpeed2)+ "," + String(fanSpeed3);
    //Serial.println(con_data);
  }
  if (startReceived == true) {

    pms1.requestRead();
    pms2.requestRead();
    pms3.requestRead();

    if(pms1.readUntil(data1)){
      d1 = data1.PM_AE_UG_2_5;
    }
    if(pms2.readUntil(data2)){
      d2 = data2.PM_AE_UG_2_5;
    }
    if(pms3.readUntil(data3)){
      d3 = data3.PM_AE_UG_2_5;
    }
    
    Serial.print(con_data);
    Serial.print(",");
    Serial.print(d1);
    Serial.print(",");
    Serial.print(d2);
    Serial.print(",");
    Serial.print(d3);
    Serial.print(",");
    
    startReceived = false;
  }
}
