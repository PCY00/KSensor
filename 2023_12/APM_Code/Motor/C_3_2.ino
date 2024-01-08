/*
이 코드는 APM에서 사용할 때 사용되는 라이브러리를 포함하여 만든 코드입니다.
해당 코드에선 1개의 bottom과 3개의 top 모터를 제어합니다.
통신은 MQTT을 활용하여 1개의 top 모터와 1개의 bottom 모터를 제어합니다.
*/
/*
#include "m_3_1.h"

m_3_1 bottom(2, 3, 4, 28);
m_3_1 top_1(5, 6, 7, 26);
m_3_1 top_2(8, 9, 10, 22);
m_3_1 top_3(11, 12, 13, 24);

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  bottom.setRound(7200);
  top_1.setRound(56000);
  top_2.setRound(56000);
  top_3.setRound(56000);

  //10ms down
  bottom.setspeed(1000);
  top_1.setspeed(300);
  top_2.setspeed(300);
  top_3.setspeed(300);

  //bottom motor setup
  bottom.BottomSetup();

  //top motor setup
  top_1.TopSetup();
  top_2.TopSetup();
  top_3.TopSetup();

  bottom.stopMotor();
  top_1.stopMotor();
  top_2.stopMotor();
  top_3.stopMotor();
  

}

void loop() {
  if (Serial.available()) {
    // 문자열을 읽어서 처리
    String receivedString = Serial.readStringUntil('\n'); // 개행 문자까지 읽기
    
    //pm1, bottom 모터를 제어할 때 사용
    //mqtt로 제어를 받으면 해당되는 모터가 점검 즉 위치를 초기화할 때 다른 모터들도 멈추고, 이후 모든 모터들이 다시 움직임
    if (receivedString.startsWith("pm1")) {
      top_1.startMotor();
      top_1.TopSetup();
      int num = receivedString.substring(4).toInt(); // 문자열에서 숫자 부분 추출
      //Serial.println(num);
      top_1.TopLocation(num);
      Serial.println(num);
    }else if (receivedString.startsWith("pm1")) {
      top_1.startMotor();
      top_1.TopSetup();
      int num = receivedString.substring(4).toInt(); // 문자열에서 숫자 부분 추출
      //Serial.println(num);
      top_1.TopLocation(num);
      Serial.println(num);
    }else if (receivedString.startsWith("pm2")) {
      top_2.startMotor();
      top_2.TopSetup();
      int num = receivedString.substring(4).toInt(); // 문자열에서 숫자 부분 추출
      //Serial.println(num);
      top_2.TopLocation(num);
      Serial.println(num);
    }else if (receivedString.startsWith("pm3")) {
      top_3.startMotor();
      top_3.TopSetup();
      int num = receivedString.substring(4).toInt(); // 문자열에서 숫자 부분 추출
      //Serial.println(num);
      top_3.TopLocation(num);
      Serial.println(num);
    }
    else if (receivedString.startsWith("bottom")){
      bottom.startMotor();
      bottom.BottomSetup();
      int num = receivedString.substring(7).toInt();
      bottom.BottomLocation(num);
      Serial.println(num);
    }
  }
  
  bottom.moveto();
  top_1.moveto();
  //top_2.moveto();
  //top_3.moveto();
  //top_1.moveto_pm();
  top_2.moveto_pm();
  top_3.moveto_pm();
}

*/

#include "m_3_1.h"

m_3_1 bottom(2, 3, 4, 28);
m_3_1 top_1(5, 6, 7, 26);
m_3_1 top_2(8, 9, 10, 22);
m_3_1 top_3(11, 12, 13, 24);

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  bottom.setRound(7200);
  top_1.setRound(56000);
  top_2.setRound(56000);
  top_3.setRound(56000);

  //10ms down
  bottom.setspeed(1000);
  top_1.setspeed(300);
  top_2.setspeed(300);
  top_3.setspeed(300);

  //bottom motor setup
  bottom.BottomSetup();

  //top motor setup
  top_1.TopSetup();
  top_2.TopSetup();
  top_3.TopSetup();

  bottom.stopMotor();
  //top_1.stopMotor();
  

}

void loop() {
  if (Serial.available()) {
    // 문자열을 읽어서 처리
    String receivedString = Serial.readStringUntil('\n'); // 개행 문자까지 읽기
    
    //pm1, bottom 모터를 제어할 때 사용
    //mqtt로 제어를 받으면 해당되는 모터가 점검 즉 위치를 초기화할 때 다른 모터들도 멈추고, 이후 모든 모터들이 다시 움직임
    if (receivedString.startsWith("pm1")) {
      top_1.startMotor();
      top_1.TopSetup();
      int num = receivedString.substring(4).toInt(); // 문자열에서 숫자 부분 추출
      Serial.println(num);
      top_1.TopLocation(num);
      
    } else if (receivedString.startsWith("bottom")){
      bottom.startMotor();
      bottom.BottomSetup();
      int num = receivedString.substring(7).toInt();
      Serial.println(num);
      bottom.BottomLocation(num);
    }
  }
  
  bottom.moveto();
  top_1.moveto_pm();
  top_2.moveto_pm();
  top_3.moveto_pm();
} 
