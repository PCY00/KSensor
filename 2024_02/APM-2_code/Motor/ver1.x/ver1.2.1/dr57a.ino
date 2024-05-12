#include "misumi.h"

#define RESOLUTION_200 200
#define RESOLUTION_400 400
#define RESOLUTION_800 800
#define RESOLUTION_1600 1600
#define RESOLUTION_3200 3200
#define RESOLUTION_6400 6400
#define RESOLUTION_12800 12800
#define RESOLUTION_25600 25600

misumi test_h(6,7,RESOLUTION_400);

String model_1 = "0";
//String model_2= "0";
//String model_3= "0";
String con_data = "";

void setup(){
  Serial.begin(57600);
  test_h.setSpeed(5000);
}

void loop(){
  if (Serial.available()) {
    // 문자열을 읽어서 처리
    String receivedString = Serial.readStringUntil('\n'); // 개행 문자까지 읽기
    
    if (receivedString.startsWith("model1")) {
      int num = receivedString.substring(7).toInt(); // 문자열에서 숫자 부분 추출
      model_1 = String(num);
      con_data = model_1;
      test_h.set_angle((long)num);
      Serial.println(con_data);
    }
    /*
    else if(receivedString.startsWith("model2")){
      top_2.startMotor();
      top_2.TopSetup();
      int num = receivedString.substring(7).toInt(); // 문자열에서 숫자 부분 추출
      pm2_save = String(num);
      con_data = bottom_save + "," + pm1_save + "," + pm2_save + "," + pm3_save;
      top_2.TopLocation(num);
      Serial2.print(con_data);   
    }else if(receivedString.startsWith("model3")){
      top_3.startMotor();
      top_3.TopSetup();
      int num = receivedString.substring(7).toInt(); // 문자열에서 숫자 부분 추출
      pm3_save = String(num);
      con_data = bottom_save + "," + pm1_save + "," + pm2_save + "," + pm3_save;
      top_3.TopLocation(num);
      Serial2.print(con_data);     
    }else if (receivedString.startsWith("bottom")){
      bottom.startMotor();
      bottom.BottomSetup();
      int num = receivedString.substring(7).toInt();
      bottom_save = String(num);
      con_data = bottom_save + "," + pm1_save + "," + pm2_save + "," + pm3_save;      
      bottom.BottomLocation(num);
      Serial2.print(con_data);
    }
    */
  }
  test_h.move_motor_angle();
}
