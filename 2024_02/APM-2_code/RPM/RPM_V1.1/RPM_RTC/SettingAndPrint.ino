#include <SoftwareSerial.h>

SoftwareSerial rtcSerial(2,3);

#define baudRate 19200
#define stx 0x02
#define Checksum 0xFF

// RTC Setup request
#define rtc_request_cmd     0x30
#define rtc_request_size_1  0x00
#define rtc_request_size_2  0x00

// RTC Setup response
#define rtc_response_cmd    0x31
#define rtc_response_size_1 0x06
#define rtc_response_size_2 0x00

// RTC Setup request
#define rtc_setup_cmd 0x32
#define rtc_setup_size_1 0x06
#define rtc_setup_size_2 0x00

//Time setting
//24.02.29 23:30:00
#define YEAR    0x18
#define MONTH   0x02
#define DAY     0x1D
#define HOUR    0x17
#define MINUTE  0x12
#define SECOND  0x00

void send_rtc_setup_request();
void send_rtc_query_request();
void receive_rtc_setup_response();

void setup() {
  Serial.begin(19200);
  rtcSerial.begin(baudRate);
  
  // RTC 설정 요청 보내기
  send_rtc_setup_request();
  delay(1000);
}

void loop() {
  // RTC 요청 보내기
  send_rtc_query_request();
  // RTC 응답 받기
  receive_rtc_setup_response();
  delay(1000); // 1초 대기
}

void send_rtc_setup_request() {
  uint8_t chsum = (Checksum - (rtc_setup_cmd + rtc_setup_size_1 + rtc_setup_size_2 + YEAR + MONTH + DAY + HOUR + MINUTE + SECOND));
  rtcSerial.write((uint8_t)stx);
  rtcSerial.write((uint8_t)rtc_setup_cmd);
  rtcSerial.write((uint8_t)rtc_setup_size_1);
  rtcSerial.write((uint8_t)rtc_setup_size_2);
  rtcSerial.write((uint8_t)YEAR);
  rtcSerial.write((uint8_t)MONTH);
  rtcSerial.write((uint8_t)DAY);
  rtcSerial.write((uint8_t)HOUR);
  rtcSerial.write((uint8_t)MINUTE);
  rtcSerial.write((uint8_t)SECOND);
  rtcSerial.write(chsum);
}

void send_rtc_query_request() {
  uint8_t chsum = (Checksum - (rtc_request_cmd + rtc_request_size_1 + rtc_request_size_2));
  rtcSerial.write((uint8_t)stx);
  rtcSerial.write((uint8_t)rtc_request_cmd);
  rtcSerial.write((uint8_t)rtc_request_size_1);
  rtcSerial.write((uint8_t)rtc_request_size_2);
  rtcSerial.write(chsum);
}

void receive_rtc_setup_response() {
  if (rtcSerial.available() >= 10) {
    uint8_t STX = rtcSerial.read();
    uint8_t CMD = rtcSerial.read();
    uint8_t SIZE_1 = rtcSerial.read();
    uint8_t SIZE_2 = rtcSerial.read();

    if (STX != stx || CMD != rtc_response_cmd) {
      Serial.println("STX or CMD error");
      return;
    }

    uint8_t data[6];
    for (int i = 0; i < 6; i++) {
      data[i] = rtcSerial.read();
    }

    uint8_t chsum = (Checksum - (rtc_response_cmd + rtc_response_size_1 + rtc_response_size_2 + data[0] + data[1]+ data[2] + data[3] + data[4] + data[5]));
    uint8_t receivedChsum = rtcSerial.read();

    if (chsum != receivedChsum) {
      Serial.println("Checksum error");
      return;
    }

    // 응답 데이터를 출력
    Serial.print("Year: "); Serial.println(data[0]);
    Serial.print("Month: "); Serial.println(data[1]);
    Serial.print("Day: "); Serial.println(data[2]);
    Serial.print("Hour: "); Serial.println(data[3]);
    Serial.print("Minute: "); Serial.println(data[4]);
    Serial.print("Second: "); Serial.println(data[5]);
    Serial.println("RTC print complete");
  }
}
