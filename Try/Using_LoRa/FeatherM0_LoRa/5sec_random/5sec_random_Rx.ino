#include <RH_RF95.h>
#include <SPI.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 920.9

RH_RF95 rf95(RFM95_CS, RFM95_INT);

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
  if(rf95.available()){
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
        if (buf[0] == 0x01) { // 송신부 1에서의 데이터
        receiveDust(buf, 1);
      }
      else if(buf[0] == 0x02){ // 송신부 2에서의 데이터
        receiveDust(buf, 2);
      }else{                  // 송신부 3에서의 데이터
        receiveDust(buf, 3);
      }
    }
    else {
      Serial.println("Receive failed");
    }
  }
}

void receiveDust(uint8_t* msg, uint8_t num){
  Serial.print("Dust_");
  Serial.print(num);
  Serial.print(": ");
  
  int16_t pm2_5;
  pm2_5 = msg[2] << 8 | msg[1];
  
  Serial.print(pm2_5);
  Serial.println();
}
