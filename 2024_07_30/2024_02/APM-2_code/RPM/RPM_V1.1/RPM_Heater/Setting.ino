#include <SoftwareSerial.h>

SoftwareSerial heaterSerial(2,3);

#define baudRate 19200
#define stx 0x02
#define Checksum 0xFF

// Heater Information Query
#define Heater_Information_Query_cmd        0x20
#define Heater_Information_Query_size_1     0x00
#define Heater_Information_Query_size_2     0x00

// Heater Information Return
#define Heater_Information_Return_cmd       0x21
#define Heater_Information_Return_size_1    0x03
#define Heater_Information_Return_size_2    0x00

//Heater Setup 
#define Heater_Setup_cmd                    0x22
#define Heater_Setup_size_1                 0x03
#define Heater_Setup_size_2                 0x00

//Heater 25도 이하 히터 on & 30도 이상 히터 off
#define Enable                              0x01
#define High_Level_Set_Value                0x1E
#define Low_Level_Set_Value                 0x14

void Heater_Information_Query();
void Heater_Information_Return();
void Heater_Setup(uint8_t high, uint8_t low);

void setup() {
  Serial.begin(19200);
  heaterSerial.begin(baudRate);

  Heater_Setup((uint8_t)High_Level_Set_Value, (uint8_t)Low_Level_Set_Value);
}

void loop() {
  Heater_Information_Query();
  delay(1000); // 1초 대기
}

void Heater_Setup(uint8_t high, uint8_t low){
  uint8_t chsum = (Checksum - (Heater_Setup_cmd + Heater_Setup_size_1 + Heater_Setup_size_2 + Enable + high + low));
  heaterSerial.write((uint8_t)stx);
  heaterSerial.write((uint8_t)Heater_Setup_cmd);
  heaterSerial.write((uint8_t)Heater_Setup_size_1);
  heaterSerial.write((uint8_t)Heater_Setup_size_2);
  heaterSerial.write((uint8_t)Enable);
  heaterSerial.write(high);
  heaterSerial.write(low);
  heaterSerial.write(chsum);

  Serial.print(chsum + (String)" Setting end \n");
  
  Heater_Information_Return();
}

void Heater_Information_Return(){
  if (heaterSerial.available() >= 8){
    uint8_t STX = heaterSerial.read();
    uint8_t CMD = heaterSerial.read();
    uint8_t SIZE_1 = heaterSerial.read();
    uint8_t SIZE_2 = heaterSerial.read();

    if(STX != stx || CMD != Heater_Information_Return_cmd){
      Serial.println("STX or CMD error");
      return;
    }

    uint8_t Data[3];
    for(int i = 0; i < 3; i++){
      Data[i] = heaterSerial.read();
    }

    uint8_t chsum = Checksum - (Heater_Information_Return_cmd + Heater_Information_Return_size_1 + Heater_Information_Return_size_2 + Data[0] + Data[1] + Data[2]);

    if(heaterSerial.read() != chsum){
      Serial.println("checkSum error");
      return;
    }

    Serial.println("success");
  }else{
    Serial.println("data error");
    return; 
  }
}

void Heater_Information_Query(){
  uint8_t chsum = (Checksum - (Heater_Information_Query_cmd + Heater_Information_Query_size_1 + Heater_Information_Query_size_2));
  heaterSerial.write((uint8_t)stx);
  heaterSerial.write((uint8_t)Heater_Information_Query_cmd);
  heaterSerial.write((uint8_t)Heater_Information_Query_size_1);
  heaterSerial.write((uint8_t)Heater_Information_Query_size_2);
  heaterSerial.write(chsum);

  Serial.println("query success");
  
  Heater_Information_Return();
}
