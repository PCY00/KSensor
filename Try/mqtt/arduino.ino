const int ledPin = 13; // 아두이노 내장 LED 핀
String receivedData;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    receivedData = Serial.readStringUntil('\n'); // 개행 문자까지의 데이터를 읽음
    receivedData.trim(); // 앞뒤 공백 제거
    
    if (receivedData == "ok") {
      digitalWrite(ledPin, HIGH); // LED 켜기
      Serial.println("HIGH");
    } else if (receivedData == "no") {
      digitalWrite(ledPin, LOW);  // LED 끄기
      Serial.println("LOW");
    }
  }
}
