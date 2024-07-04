#include <PMS.h>

int d0;
bool startReceived = false;

PMS pms(Serial1);
PMS::DATA data;

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);

  pms.passiveMode();
}

void loop(){
  while(!startReceived){
    if(Serial.available() > 0){
      String Input = Serial.readStringUntil('\n');
      if(Input == "start"){
        startReceived = true;
      }
    }
  }
  if(startReceived == true){
    pms.requestRead();
    if(pms.readUntil(data)){
      d0 = data.PM_AE_UG_2_5;
      Serial.println(d0);

      startReceived = false;
    }
  }
}
