#include <PMS.h>
#include <SoftwareSerial.h>

SoftwareSerial nockanda(2,3); //tx=2, rx=3

PMS pms(nockanda);
PMS::DATA data;

bool runEvery(unsigned long interval);

void setup()
{
  Serial.begin(9600);
  nockanda.begin(9600);
  pms.passiveMode(); 
}

uint16_t d0;

void loop()
{
  if(runEvery(5000)){
    pms.requestRead();

    if(pms.readUntil(data)){
      d0 = data.PM_AE_UG_2_5;
    }

    
    Serial.println(d0);
  }
}

bool runEvery(unsigned long interval){
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
