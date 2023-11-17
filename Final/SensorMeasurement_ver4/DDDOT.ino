#include <PMS.h>                //pms7003
#include "DHT_T.h"              //DHT22
#include "DFRobot_OzoneSensor.h"
#include "RTClib.h"

RTC_DS3231 rtc;

#define DHTPIN 6              // Digital pin connected to the DHT sensor

DHT dht(DHTPIN, DHT22);

#define COLLECT_NUMBER   20              // collect number, the collection range is 1-100
#define Ozone_IICAddress OZONE_ADDRESS_3

DFRobot_OzoneSensor Ozone;

PMS pms1(Serial3);
PMS::DATA data1;

PMS pms2(Serial2);
PMS::DATA data2;

PMS pms3(Serial1);
PMS::DATA data3;

uint16_t d1, d2, d3;
//dht var
float temp, humi;
unsigned long previousMillis = 0;
const unsigned long interval = 30000;  // 5분 간격


void setup() {
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  Serial.begin(9600);
  delay(1000);

  Ozone.begin(Ozone_IICAddress);
  Ozone.setModes(MEASURE_MODE_PASSIVE);

  pms1.passiveMode();
  pms2.passiveMode();
  pms3.passiveMode();

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

  //Serial.print("DataType: ");
  //Serial.println("NPMDust Dust1 Dust2 Dust3 temp humi Ozone");
}

void loop() {
  DateTime now = rtc.now();
  unsigned long currentMillis = now.unixtime() * 1000;
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    int ozoneConcentration = Ozone.readOzoneData(COLLECT_NUMBER);

    pms1.requestRead();
    pms2.requestRead();
    pms3.requestRead();

    if(pms1.readUntil(data1)){
      d1 = data1.PM_AE_UG_2_5;
    }
    if(pms2.readUntil(data2)){
      d2 = data2.PM_AE_UG_2_5;
    }
    if(pms3.readUntil(data3)){
      d3 = data3.PM_AE_UG_2_5;
    }

    switch(dht.read()){
      case DHT_OK:
        humi = dht.humidity;
        temp = dht.temperature;
        break;
      case DHT_ERR_CHECK:
          humi = 0.0;
          temp = 0.0;
          break;
      case DHT_ERR_TIMEOUT:
          humi = 0.0;
          temp = 0.0;
          break;
      default:
          humi = 0.0;
          temp = 0.0;
          break;
    }
    float Ozone_ppm = (float)ozoneConcentration / 1000;

    Serial.print(d1);
    Serial.print(",");
    Serial.print(d2);
    Serial.print(",");
    Serial.print(d3);
    Serial.print(",");
    Serial.print(temp);
    Serial.print(",");
    Serial.print(humi);
    Serial.print(",");
    Serial.println(Ozone_ppm, 3);
  }
}
