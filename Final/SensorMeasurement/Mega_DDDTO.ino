D#include "PMS.h"                //pms7003
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

char buffer[50];
unsigned long past = 0;
bool flag = false;
//pms var
int d1, d2, d3;
//dht var
float temp, humi;
char temp_print[7];
char humi_print[7];
char Ozone_print[7];  //print Ozone string

void setup() {
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  Serial.begin(9600);
//==================================================================================

  Ozone.begin(Ozone_IICAddress);
  
  Ozone.setModes(MEASURE_MODE_PASSIVE);
}

void loop() {
  unsigned long now = millis();

  int ozoneConcentration = Ozone.readOzoneData(COLLECT_NUMBER);
//==================================================================================
// 5sec check
  if(now - past >= 5000){
    past = now;
    flag = true;
  }
//==================================================================================
// pms val read
  if (pms1.read(data1)){
    d1 = data1.PM_AE_UG_2_5;
  }
  if (pms2.read(data2)){
    d2 = data2.PM_AE_UG_2_5;
  }
  if(pms3.read(data3)){
    d3 = data3.PM_AE_UG_2_5;
  }

//==================================================================================
// dht val read
  switch(dht.read())
  {
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

//==================================================================================
// After 5sec print
  if(flag == true){
    float Ozone_ppm = (float)ozoneConcentration / 1000;
    
    dtostrf(temp, 5, 2, temp_print);
    dtostrf(humi, 5, 2, humi_print);
    dtostrf(Ozone_ppm, 5, 3, Ozone_print);
    sprintf(buffer,"%4d,%4d,%4d, %s, %s, %s", d1, d2, d3, temp_print, humi_print, Ozone_print);
    Serial.println(buffer);

    flag = false;
  }

  Serial.flush();
}
