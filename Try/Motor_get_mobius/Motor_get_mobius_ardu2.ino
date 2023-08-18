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
    // 문자열을 읽어서 처리
    String receivedString = Serial.readStringUntil('\n'); // 개행 문자까지 읽기
    
    // 'pm1' 또는 'pm2' 문자열이 수신되면 내장 LED 제어
    if (receivedString.startsWith("pm1")) {
      int num = receivedString.substring(4).toInt(); // 문자열에서 숫자 부분 추출
      blinkLED(num);
    } else if (receivedString.startsWith("pm2")) {
      // 아무 동작하지 않음
    } else if (receivedString.startsWith("pm3")){
      
    } else if (receivedString.startsWith("bottom")){
      
    }
  }
}

// 내장 LED를 주어진 횟수만큼 깜빡이는 함수
void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);  // LED 켜기
    delay(1000);  // 1000ms 대기 (1초)
    digitalWrite(ledPin, LOW);   // LED 끄기
    delay(1000);  // 1000ms 대기 (1초)
  }
}
