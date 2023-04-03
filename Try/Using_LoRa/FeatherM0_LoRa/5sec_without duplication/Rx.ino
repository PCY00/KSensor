//https://www.airspayce.com/mikem/arduino/RadioHead/classRHGenericDriver.html#aa1c0a94ba6414b5bbab0133c4d74e646
#include <RH_RF95.h>
#include <SPI.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 920.9

RH_RF95 rf95(RFM95_CS, RFM95_INT);



uint16_t saveDust[3] = {0};
const uint8_t startCode[3] = {0x01,0x02,0x03};
bool txStatus[] = {true, false, false};

bool runEvery(unsigned long interval);
void receiveDust(uint8_t* msg, uint8_t num);

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(100);

  Serial.println("Feather LoRa RX Test!");
  
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);
}

void loop() {
  if(runEvery(5000)){
    String dataString = "";

    for(uint8_t i = 0; i < 3; i++){
      if(txStatus[i]){
        rf95.send(&startCode[i], 1);
        rf95.waitPacketSent();

        if(rf95.available()){
          uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
          uint8_t len = sizeof(buf);
          
          if(rf95.recv(buf, &len)){
            receiveDust(buf, i + 1);
            delay(10);
          }
        }
        txStatus[i] = false;
        txStatus[(i+1)%3] = true;
      }
      delay(100);
    }

    for(uint8_t i = 0; i < 3; i++){
      dataString += saveDust[i];
      dataString += " ";
    }
    Serial.println(dataString);
  }
  delay(100);
}

void receiveDust(uint8_t* msg, uint8_t num){
  int16_t pm2_5 = msg[1] << 8 | msg[0];
  //save value
  saveDust[num - 1] = pm2_5;
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
