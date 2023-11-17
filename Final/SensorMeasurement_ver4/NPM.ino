#include <PMS.h>
#include <SoftwareSerial.h>
#include "RTClib.h"

RTC_DS3231 rtc;

SoftwareSerial nockanda(2,3); //tx=2, rx=3

PMS pms(nockanda);
PMS::DATA data;

unsigned long previousMillis = 0;
const unsigned long interval = 30000;  // 5분 간격

void setup()
{
  Serial.begin(9600);
  nockanda.begin(9600);
  pms.passiveMode();

  //젯슨에서 값 받는거 대기
  while (!startReceived) {
  // start 신호 받기
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      if (input == "start") {
        startReceived = true;
      }
    }
  }

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

uint16_t d0;

void loop(){
  DateTime now = rtc.now();
  unsigned long currentMillis = now.unixtime() * 1000;
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    pms.requestRead();

    if(pms.readUntil(data)){
      d0 = data.PM_AE_UG_2_5;
    }
    Serial.println(d0);
  }
}
