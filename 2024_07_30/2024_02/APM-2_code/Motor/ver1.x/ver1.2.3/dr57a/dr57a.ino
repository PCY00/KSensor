#include "misumi.h"

#define RESOLUTION_200 200
#define RESOLUTION_400 400
#define RESOLUTION_800 800
#define RESOLUTION_1600 1600
#define RESOLUTION_3200 3200
#define RESOLUTION_6400 6400
#define RESOLUTION_12800 12800
#define RESOLUTION_25600 25600

//module1
int switch1_0 = 22;
int switch1_30 = 23;
int switch1_60 = 24;
int switch1_90 = 25;

//module2
int switch2_0 = 26;
int switch2_30 = 27;
int switch2_60 = 28;
int switch2_90 = 29;

//module3
int switch3_0 = 30;
int switch3_30 = 31;
int switch3_60 = 32;
int switch3_90 = 33;


misumi test_1(6,7,RESOLUTION_400, switch1_0, switch1_30, switch1_60, switch1_90);
misumi test_2(8,9,RESOLUTION_400, switch2_0, switch2_30, switch2_60, switch2_90);
misumi test_3(10,11,RESOLUTION_400, switch3_0, switch3_30, switch3_60, switch3_90);

String model_1 = "0";
String model_2= "0";
String model_3= "0";
String con_data = "0,0,0";

void setup(){
  Serial.begin(9600);
  
  test_1.setSpeed(5000);
  test_2.setSpeed(5000);
  test_3.setSpeed(5000);
}

void loop(){ 
  if (Serial.available()) {
    String receivedString = Serial.readStringUntil('\n');
    
    if (receivedString.startsWith("bottom1")) {
      int num = receivedString.substring(8).toInt();
      model_1 = String(num);
      con_data = model_1;
      //test_1.set_angle((long)num);
      test_1.set_angle((long)1080);
      test_1.switch_set(num);
      //Serial.println(con_data);
    }else if (receivedString.startsWith("bottom2")) {
      int num = receivedString.substring(8).toInt();
      model_2 = String(num);
      con_data = model_2;
      test_2.set_angle((long)num);
      //Serial.println(con_data);
    }else if (receivedString.startsWith("bottom3")) {
      int num = receivedString.substring(8).toInt();
      model_3 = String(num);
      con_data = model_3;
      test_3.set_angle((long)num);
      //Serial.println(con_data);
    }else if(receivedString == "start"){
      Serial.println(con_data);
    }

    con_data = model_1 + "," + model_2 + "," + model_3;
  }
  
  test_1.move_motor_angle();
  test_2.move_motor_angle();
  test_3.move_motor_angle();

}
