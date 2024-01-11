#include <SPI.h>
#include <WiFi101.h>
#include "m_3_1.h"

char ssid[] = "";  // 여기에 WiFi SSID를 입력하세요
char pass[] = "";  // 여기에 WiFi 비밀번호를 입력하세요
int status = WL_IDLE_STATUS;

char server[] = "114.71.220.59";
int port = 7579;

WiFiClient client;

String pm_save= "0";
String bottom_save = "0";
String con_data = "";

m_3_1 bottom(2, 3, 4, 28);
m_3_1 top_1(5, 6, 7, 26);
m_3_1 top_2(8, 9, 10, 22);
m_3_1 top_3(11, 12, 13, 24);

void connectWiFi();
void sendPOSTRequest();

void setup() {
  connectWiFi();
  
  bottom.setRound(7200);
  top_1.setRound(56000);
  top_2.setRound(56000);
  top_3.setRound(56000);

  //10ms down
  bottom.setspeed(1000);
  top_1.setspeed(300);
  top_2.setspeed(300);
  top_3.setspeed(300);

  //bottom motor setup
  bottom.BottomSetup();

  //top motor setup
  top_1.TopSetup();
  top_2.TopSetup();
  top_3.TopSetup();

  bottom.stopMotor();
  //top_1.stopMotor();
  

}

void loop() {
  if (Serial.available()) {
    // 문자열을 읽어서 처리
    String receivedString = Serial.readStringUntil('\n'); // 개행 문자까지 읽기
    
    //pm1, bottom 모터를 제어할 때 사용
    //mqtt로 제어를 받으면 해당되는 모터가 점검 즉 위치를 초기화할 때 다른 모터들도 멈추고, 이후 모든 모터들이 다시 움직임
    if (receivedString.startsWith("pm1")) {
      top_1.startMotor();
      top_1.TopSetup();
      int num = receivedString.substring(4).toInt(); // 문자열에서 숫자 부분 추출
      pm_save = String(num);
      Serial.println(num);
      top_1.TopLocation(num);
      
    } else if (receivedString.startsWith("bottom")){
      bottom.startMotor();
      bottom.BottomSetup();
      int num = receivedString.substring(7).toInt();
      bottom_save = String(num);
      Serial.println(num);
      bottom.BottomLocation(num);
    }
  }
  
  bottom.moveto();
  top_1.moveto_pm();
  top_2.moveto_pm();
  top_3.moveto_pm();
}

void connectWiFi() {
  WiFi.setPins(35, 37, 39);
  while (status != WL_CONNECTED) {
    Serial.begin(9600);
    Serial.print("Attempting to connect to WiFi...");
    status = WiFi.begin(ssid, pass);
    delay(3000);
  }
  Serial.println("Connected to WiFi");
}

void sendPOSTRequest() {
  if (client.connect(server, port)) {
    String data = "{\"m2m:cin\": {\"con\": \"" + con_data + "\"}}"; // con_data 변수의 값을 데이터에 포함
    
    String url = "/Mobius/PCY_Home/Value";
    
    client.print("POST " + url + " HTTP/1.1\r\n");
    client.print("Host: " + String(server) + "\r\n");
    client.print("Accept: application/json\r\n");
    client.print("X-M2M-RI: 12345\r\n");
    client.print("X-M2M-Origin: SZ8V742I5tw\r\n");
    client.print("Content-Type: application/vnd.onem2m-res+json; ty=4\r\n");
    client.print("Content-Length: ");
    client.print(data.length());
    client.print("\r\n\r\n");
    client.print(data);
    
    delay(1000);
  } else {
    Serial.println("Connection failed");
  }
}
