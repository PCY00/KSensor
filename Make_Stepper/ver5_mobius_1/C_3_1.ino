#include "m_3_1.h"

m_3_1 bottom(2, 3, 4, 28);
m_3_1 top_1(5, 6, 7, 26);
m_3_1 top_2(8, 9, 10, 22);
m_3_1 top_3(11, 12, 13, 24);

void setup() {
  // put your setup code here, to run once:
  bottom.setRound(7200);
  top_1.setRound(56000);
  top_2.setRound(56000);
  top_3.setRound(56000);

  //10ms down
  bottom.setspeed(300);
  top_1.setspeed(300);
  top_2.setspeed(300);
  top_3.setspeed(300);

  //bottom motor setup
  bottom.MotorSetup();

  //top motor setup
  top_1.MotorSetup();
  top_2.MotorSetup();
  top_3.MotorSetup();

}

void loop() {
  if (Serial.available()) {
    // 문자열을 읽어서 처리
    String receivedString = Serial.readStringUntil('\n'); // 개행 문자까지 읽기
    
    //pm1, bottom 모터를 제어할 때 사용
    //mqtt로 제어를 받으면 해당되는 모터가 점검 즉 위치를 초기화할 때 다른 모터들도 멈추고, 이후 모든 모터들이 다시 움직임
    if (receivedString.startsWith("pm1")) {
      top_1.MotorSetup();
      int num = receivedString.substring(4).toInt(); // 문자열에서 숫자 부분 추출
      top_1.setRound(num);
      
    } else if (receivedString.startsWith("bottom")){
      bottom.MotorSetup();
      int num = receivedString.substring(4).toInt();
      bottom.setRound(num);
    }
  }
  bottom.moveto();
  top_1.moveto();
  top_2.moveto();
  top_3.moveto();
}
