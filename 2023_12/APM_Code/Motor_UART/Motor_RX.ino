//받는 쪽
String save = "";

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop(){
  if(Serial1.available()){
    save = Serial1.readStringUntil('\n'); // 개행 문자까지 읽기
  }
  Serial.println(save);
  delay(3000);
}
