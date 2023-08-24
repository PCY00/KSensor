#include <PMS.h>                //pms7003
#include "DHT_T.h"              //DHT22
#include "DFRobot_OzoneSensor.h"

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

bool runEvery(unsigned long interval);

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

  Serial.print("DataType: ");
  Serial.println("NPMDust Dust1 Dust2 Dust3 temp humi Ozone");
}

void loop() {

  if(runEvery(5000)){
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

bool runEvery(unsigned long interval){
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
