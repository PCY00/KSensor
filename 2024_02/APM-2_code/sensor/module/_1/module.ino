#include <PMS.h>                //pms7003

PMS pms1(Serial3);
PMS::DATA data1;

PMS pms2(Serial2);
PMS::DATA data2;

PMS pms3(Serial1);
PMS::DATA data3;

uint16_t d1, d2, d3;
const int fanPin = 9; // 팬을 제어하는 핀
const int maxSpeed = 255; // 최대 속도 (0-255)
int fanSpeed = 0; // 현재 팬 속도
String con_data = "";

bool startReceived = false;

void setup() {
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  Serial.begin(9600);
  delay(1000);

  pinMode(fanPin, OUTPUT);
  
  pms1.passiveMode();
  pms2.passiveMode();
  pms3.passiveMode();

  
}

void loop() {
  //젯슨에서 값 받는거 대기
  while (!startReceived) {
  // start 신호 받기
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      if (input.startsWith("p_")) {
        fanSpeed = input.substring(2).toInt(); // 문자열에서 숫자 부분 추출
        
        analogWrite(fanPin, fanSpeed);
        
        con_data = String(fanSpeed);
        Serial.println(con_data);
        startReceived = true;
      }
    }
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

    Serial.print(d1);
    Serial.print(",");
    Serial.print(d2);
    Serial.print(",");
    Serial.print(d3);
    Serial.print(",");

    startReceived = false;
  }
}
