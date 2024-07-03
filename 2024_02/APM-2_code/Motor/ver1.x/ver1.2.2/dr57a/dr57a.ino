#include "misumi.h"

#define RESOLUTION_200 200
#define RESOLUTION_400 400
#define RESOLUTION_800 800
#define RESOLUTION_1600 1600
#define RESOLUTION_3200 3200
#define RESOLUTION_6400 6400
#define RESOLUTION_12800 12800
#define RESOLUTION_25600 25600

misumi test_1(6,7,RESOLUTION_400);
misumi test_2(8,9,RESOLUTION_400);
misumi test_3(10,11,RESOLUTION_400);

String model_1 = "0";
String model_2= "0";
String model_3= "0";
String con_data = "";

void setup(){
  Serial.begin(57600);
  test_h.setSpeed(5000);
}

void loop(){
  if (Serial.available()) {
    // 문자열을 읽어서 처리
    String receivedString = Serial.readStringUntil('\n'); // 개행 문자까지 읽기
    
    if (receivedString.startsWith("bottom1")) {
      int num = receivedString.substring(8).toInt(); // 문자열에서 숫자 부분 추출
      model_1 = String(num);
      con_data = model_1;
      test_1.set_angle((long)num);
      Serial.println(con_data);
    }else if (receivedString.startsWith("bottom2")) {
      int num = receivedString.substring(8).toInt(); // 문자열에서 숫자 부분 추출
      model_2 = String(num);
      con_data = model_2;
      test_2.set_angle((long)num);
      Serial.println(con_data);
    }else if (receivedString.startsWith("bottom3")) {
      int num = receivedString.substring(8).toInt(); // 문자열에서 숫자 부분 추출
      model_3 = String(num);
      con_data = model_3;
      test_3.set_angle((long)num);
      Serial.println(con_data);
    }
  }
  test_1.move_motor_angle();
  test_2.move_motor_angle();
  test_3.move_motor_angle();
}
