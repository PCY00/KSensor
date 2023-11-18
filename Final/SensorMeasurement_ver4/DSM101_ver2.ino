/*
 * 6초에 한번 씩 미세먼지 측정을 하는 모듈
 * 따라서 6초에 들어오는 값을 50번 더하여 50을 나눈 값 출력
 * 5분 마다 값 출력
 */

#include <SoftwareSerial.h>
#include "RTClib.h"

RTC_DS3231 rtc;

SoftwareSerial pms(8,9); //RX TX
const byte STX = 0x02;
const byte CMD_Value_Query = 0x10;
const byte CMD_Value_Return = 0x11;
const byte CHECKSUM = 0xFF;

bool startReceived = false;

void send_value_query();
int parse_value_return();
void Data_show();

byte Data[24];
int save = 0;       //50개 인지 확인용
int ByteSum[3];    //byte 합

unsigned long previousMillis = 0;
const unsigned long interval = 5000;  // 5분 간격

void setup() {
  Serial.begin(19200);
  pms.begin(19200);
  //Serial.println("PM1 PM2.5 PM10");
  //초기 설정
  for(int i = 0 ; i < 3; i++){
    ByteSum[i] = 0;
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
}

void loop() {
  DateTime now = rtc.now();
  unsigned long currentMillis = now.unixtime() * 1000;
  
  if (currentMillis - previousMillis >= interval) {
   previousMillis = currentMillis;
   send_value_query();
   int result = parse_value_return();
   //Serial.println(save);
   if(save == 50){
     //Serial.println("come");
     Data_show();
     save = 0;
     for(int i = 0 ; i < 3; i++){
       ByteSum[i] = 0;
     }
   } 
 }
}

//요청
void send_value_query(){
  byte checksum = CHECKSUM - CMD_Value_Query; //데이터 시트 참고 0xEF
  pms.write(STX);
  pms.write(CMD_Value_Query);
  pms.write((byte)0x00);
  pms.write((byte)0x00);
  pms.write(checksum);

  //pms Data 초기화
  for(int i = 0 ; i < 24; i++){
    Data[i] = 0x00;
  }
}

// 응답 받기
int parse_value_return(){
  //총 27byte
  if(pms.available() >= 27){
    byte stx = pms.read();    //read()는 1바이트 읽어옴
    byte cmd = pms.read();
    byte size_1 = pms.read();
    byte size_2 = pms.read();

    if(stx != STX || cmd != CMD_Value_Return){
      return -1; //이상한 패킷
    }

    //data 읽어오기
    Data[24];
    for(int i = 0; i < 24; i++){
      Data[i] = pms.read();
    }

    byte recived_checksum = pms.read();

    //데이터 읽기 끝 이제부터 출력
    //checksum 비교
    byte cal_checksum = CHECKSUM - cmd - size_1 - size_2;
    for(int i = 0; i < 24; i++){
      cal_checksum -= Data[i];
    }
    if(recived_checksum != cal_checksum){
      return -2; //결과값 오류
    }

    for(int i = 0, j = 0; i < 6; i += 2, j++){
      ByteSum[j] += ((Data[i+1] << 8) | Data[i]);
    }
    //Serial.print("test: ");
    //for(int i = 0; i < 3; i++){
    //  Serial.print(ByteSum[i]);
    //  Serial.print(" ");
    //}
    //Serial.println();
    save++;
    return 0;//성공
  }

  return -3; //데이터 못받음
}

void Data_show(){
  /*Serial.print(Data[0]);
  for(int i = 2; i < 6; i+=2){
    Serial.print(",");
    Serial.print(Data[i]);
  }
  Serial.println();
  */

  Serial.print(ByteSum[0]/50);
  for(int i = 1; i < 3; i++){
    Serial.print(",");
    Serial.print(ByteSum[i] / 50);
  }
  Serial.println();
}
