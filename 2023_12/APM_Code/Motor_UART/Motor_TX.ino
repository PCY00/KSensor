// 보내는쪽

String pm_save= "3";
String bottom_save = "0";
String con_data = "";

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop(){
  if (Serial.available()) {
    // 문자열을 읽어서 처리
    String receivedString = Serial.readStringUntil('\n'); // 개행 문자까지 읽기
    
    if (receivedString.startsWith("pm1")) {
      int num = receivedString.substring(4).toInt(); // 문자열에서 숫자 부분 추출
      pm_save = String(num);
      con_data = bottom_save + "," + pm_save;
      Serial1.print(con_data);Serial.println(con_data);
    }else if (receivedString.startsWith("bottom")){
      int num = receivedString.substring(7).toInt();
      bottom_save = String(num);
      con_data = bottom_save + "," + pm_save;
      Serial1.print(con_data);Serial.println(con_data);
    }
  }
}
