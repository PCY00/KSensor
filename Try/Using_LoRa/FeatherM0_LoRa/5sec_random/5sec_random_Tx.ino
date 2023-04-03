#include <RH_RF95.h>
#include <SPI.h>
#include <PMS.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ_1 920.9

RH_RF95 rf95(RFM95_CS, RFM95_INT);
PMS pms(Serial1);
PMS::DATA data;

uint8_t msg[3] = {0,};

boolean runEvery(unsigned long interval);
void dataProcessing(int16_t pm2_5data);

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  
  Serial.begin(115200);
  Serial1.begin(9600);

  pms.passiveMode();
//pms.activeMode();  
  
  while(!Serial){
    delay(1);
  }
  delay(100);

  Serial.println("Tx test");
  
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  
  // 첫 번째 드라이버 설정
  while(!rf95.init()){
    Serial.println("LoRa radio init failed");
    while(1);
  }
  Serial.println("LoRa radio init success");
  
  if (!rf95.setFrequency(RF95_FREQ_1)) {
    Serial.println("setFrequency failed");  
    while(1);
  }
  Serial.print("Using F"); Serial.println(RF95_FREQ_1);
  
  rf95.setTxPower(23, false);
}

void loop(){
  if(runEvery(5000 + random(0,1000))){
    pms.requestRead();
    
    if(pms.readUntil(data)){
      int16_t pm2_5 = data.PM_AE_UG_2_5;
      dataProcessing(pm2_5);
       
      rf95.send(msg, 3);
      rf95.waitPacketSent();
    }
  }
}

boolean runEvery(unsigned long interval){
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

void dataProcessing(int16_t pm2_5data){
  msg[0] = 0x01;
  msg[1] = pm2_5data & 0xFF;
  msg[2] = (0xff & (pm2_5data >> 8));
}
