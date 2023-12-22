#include <SoftwareSerial.h>

SoftwareSerial pms(8,9); //RX TX
const byte STX = 0x02;
const byte CMD_Value_Query = 0x10;
const byte CMD_Value_Return = 0x11;
const byte CHECKSUM = 0xFF;

void send_value_query();
int parse_value_return();
void Data_show();

bool startReceived = false;
unsigned long startTime = 0;

byte Data[24];
int ByteSum[3];

bool runEvery(unsigned long interval);

void setup() {
  Serial.begin(19200);
  pms.begin(19200);
  for(int i = 0 ; i < 3; i++){
    ByteSum[i] = 0;
  }
}

void loop() {
  //젯슨에서 값 받는거 대기
  while (!startReceived) {
    // start 신호 받기
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      if (input == "start") {
        startReceived = true;
        startTime = millis(); // 'start'가 true가 된 시간 저장
      }
    }
  }
  if (startReceived == true) {
    unsigned long currentTime = millis();
    if(currentTime - startTime >= 6000){
      send_value_query();
      int result = parse_value_return();
      Data_show();
      for(int i = 0 ; i < 3; i++){
        ByteSum[i] = 0;
      }
      startReceived = false;
    }
  }
}

void send_value_query(){
  byte checksum = CHECKSUM - CMD_Value_Query;
  pms.write(STX);
  pms.write(CMD_Value_Query);
  pms.write((byte)0x00);
  pms.write((byte)0x00);
  pms.write(checksum);

  for(int i = 0 ; i < 24; i++){
    Data[i] = 0x00;
  }
}

int parse_value_return(){
  if(pms.available() >= 27){
    byte stx = pms.read();
    byte cmd = pms.read();
    byte size_1 = pms.read();
    byte size_2 = pms.read();

    if(stx != STX || cmd != CMD_Value_Return){
      return -1;
    }

    for(int i = 0; i < 24; i++){
      Data[i] = pms.read();
    }

    byte recived_checksum = pms.read();

    byte cal_checksum = CHECKSUM - cmd - size_1 - size_2;
    for(int i = 0; i < 24; i++){
      cal_checksum -= Data[i];
    }
    if(recived_checksum != cal_checksum){
      return -2;
    }

    for(int i = 0, j = 0; i < 6; i += 2, j++){
      ByteSum[j] += ((Data[i+1] << 8) | Data[i]);
    }
    return 0;
  }
  return -3;
}

void Data_show(){
  Serial.print(ByteSum[0]);
  for(int i = 1; i < 3; i++){
    Serial.print(",");
    Serial.print(ByteSum[i]);
  }
  Serial.println();
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
