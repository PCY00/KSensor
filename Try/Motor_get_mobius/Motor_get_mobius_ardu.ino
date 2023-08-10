const int ledPin = 13;  // 내장 LED가 연결된 핀 번호

void setup() {
  // 시리얼 통신 초기화
  Serial.begin(9600);
  
  // 내장 LED를 출력 핀으로 설정
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 시리얼 버퍼에 데이터가 있으면
  if (Serial.available()) {
    // 문자열을 읽어서 숫자로 변환하여 내장 LED 제어
    String receivedString = Serial.readStringUntil('\n'); // 개행 문자까지 읽기
    int receivedNum = receivedString.toInt();  // 문자열을 숫자로 변환
    
    // 입력된 숫자만큼 내장 LED를 깜빡이기
    for (int i = 0; i < receivedNum; i++) {
      digitalWrite(ledPin, HIGH);  // LED 켜기
      delay(1000);  // 1000ms 대기 (1초)
      digitalWrite(ledPin, LOW);   // LED 끄기
      delay(1000);  // 1000ms 대기 (1초)
    }
  }
}
