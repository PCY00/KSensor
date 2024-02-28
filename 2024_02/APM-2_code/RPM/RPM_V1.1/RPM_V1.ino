#include "RPM_v1.h"

RPM_V1 pms(2,3);

float five_check[3];
int check_num = 0;

void setup(){
  Serial.begin(19200);

  //5분 초기화
  for(uint8_t i = 0; i < 3; i++){
    five_check[i] = 0;
  }
}

void loop(){
  if(pms.runEvery(60000)){
     pms.Value_Query();
     int result = pms.value_Return();
     if(result == 0){
      if(check_num == 5){
        for(uint8_t i = 0; i < 3; i++){
          Serial.print(five_check[i] / 5);
          Serial.print(",");
        }
        Serial.println();
        //value reset
        for(uint8_t i = 0; i < 3; i++){
          five_check[i] = 0;
        }
        check_num = 0;
      }else{
        for(uint8_t i = 0; i < 3; i++){
          five_check[i] += (float)pms.get_data(i+3);
        }
        check_num++; 
      }
     }else if(result == -1){
      Serial.println("stx | cmd error");
     }else if(result == -2){
      Serial.println("checkSum error");
     }else if(result == -3){
      Serial.println("data error");
     }
  }
}
