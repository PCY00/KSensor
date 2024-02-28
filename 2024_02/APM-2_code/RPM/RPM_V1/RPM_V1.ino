#include "RPM_v1.h"

RPM_V1 pms(2,3);

void setup(){
  Serial.begin(19200);
}

void loop(){
  if(pms.runEvery(1000)){
     pms.Value_Query();
     int result = pms.value_Return();
     if(result == 0){
      String Data = pms.show_data();
      Serial.println(Data);
     }else{
      Serial.println("error");
     }
  }
}
