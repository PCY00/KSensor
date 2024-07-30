#include <PMS.h>

#define Speed_1 40
#define Speed_2 60
#define Speed_3 80
#define Speed_4 100
#define Speed_5 120
#define Speed_6 140
#define Speed_7 160

const int pwmPin_1 = 8;
const int pwmPin_2 = 9;
const int pwmPin_3 = 10;

const int tachPin_1 = 2;
const int tachPin_2 = 3;
const int tachPin_3 = 21;

volatile unsigned long tachCounter_1 = 0;
volatile unsigned long tachCounter_2 = 0;
volatile unsigned long tachCounter_3 = 0;

unsigned long lastTime = 0;

unsigned long last_rpm_1 = 0;
unsigned long last_rpm_2 = 0;
unsigned long last_rpm_3 = 0;

unsigned long rpm_1 = 0;
unsigned long rpm_2 = 0;
unsigned long rpm_3 = 0;

int fanSpeed1 = 0;
int fanSpeed2 = 0;
int fanSpeed3 = 0;

int ck_1 = 0;
int ck_2 = 0;
int ck_3 = 0;

String con_data = "0,0,0";

PMS pms1(Serial1);
PMS::DATA data1;

PMS pms2(Serial2);
PMS::DATA data2;

PMS pms3(Serial3);
PMS::DATA data3;

bool startReceived = false;

bool pwmChanged_1 = false;
bool pwmChanged_2 = false;
bool pwmChanged_3 = false;

bool pwmChangedCheck_1 = false;
bool pwmChangedCheck_2 = false;
bool pwmChangedCheck_3 = false;

uint16_t d1,d2,d3;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);

  delay(100);
  
  pinMode(tachPin_1, INPUT_PULLUP);
  pinMode(tachPin_2, INPUT_PULLUP);
  pinMode(tachPin_3, INPUT_PULLUP);
  
  pinMode(pwmPin_1, OUTPUT);
  pinMode(pwmPin_2, OUTPUT);
  pinMode(pwmPin_3, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(tachPin_1), tachCounterISR_1, FALLING);
  attachInterrupt(digitalPinToInterrupt(tachPin_2), tachCounterISR_2, FALLING);
  attachInterrupt(digitalPinToInterrupt(tachPin_3), tachCounterISR_3, FALLING);
      
  analogWrite(pwmPin_1, fanSpeed1);
  analogWrite(pwmPin_2, fanSpeed2);
  analogWrite(pwmPin_3, fanSpeed3);

  pms1.passiveMode();
  pms2.passiveMode();
  pms3.passiveMode();
}

//M1_P1_70

void loop() {
  if(Serial.available()){
    String input = Serial.readStringUntil('\n');
    if (input.startsWith("M3_P")) {
      if(input.substring(4,5).toInt() == 1){
        fanSpeed1 = input.substring(6).toInt();
        ck_1 = 0;
        pwmChanged_1 = true;
        switch(fanSpeed1){
          case Speed_1:
            analogWrite(pwmPin_1, Speed_1);
            break;
          case Speed_2:
            analogWrite(pwmPin_1, Speed_2);
            break;
          case Speed_3:
            analogWrite(pwmPin_1, Speed_3);
            break;
          case Speed_4:
            analogWrite(pwmPin_1, Speed_4);
            break;
          case Speed_5:
            analogWrite(pwmPin_1, Speed_5);
            break;
          case Speed_6:
            analogWrite(pwmPin_1, Speed_6);
            break;
          case Speed_7:
            analogWrite(pwmPin_1, Speed_7);
            break;
          default:
            //Serial.println("error");
            break;
        }
      }else if(input.substring(4,5).toInt() == 2){
        fanSpeed2 = input.substring(6).toInt();
        ck_2 = 0;
        pwmChanged_2 = true;
        switch(fanSpeed2){
          case Speed_1:
            analogWrite(pwmPin_2, Speed_1);
            break;
          case Speed_2:
            analogWrite(pwmPin_2, Speed_2);
            break;
          case Speed_3:
            analogWrite(pwmPin_2, Speed_3);
            break;
          case Speed_4:
            analogWrite(pwmPin_2, Speed_4);
            break;
          case Speed_5:
            analogWrite(pwmPin_2, Speed_5);
            break;
          case Speed_6:
            analogWrite(pwmPin_2, Speed_6);
            break;
          case Speed_7:
            analogWrite(pwmPin_2, Speed_7);
            break;
          default:
            //Serial.println("error");
            break;
        }
      }else if(input.substring(4,5).toInt() == 3){
        fanSpeed3 = input.substring(6).toInt();
        ck_3 = 0;
        pwmChanged_3 = true;
        switch(fanSpeed3){
          case Speed_1:
            analogWrite(pwmPin_3, Speed_1);
            break;
          case Speed_2:
            analogWrite(pwmPin_3, Speed_2);
            break;
          case Speed_3:
            analogWrite(pwmPin_3, Speed_3);
            break;
          case Speed_4:
            analogWrite(pwmPin_3, Speed_4);
            break;
          case Speed_5:
            analogWrite(pwmPin_3, Speed_5);
            break;
          case Speed_6:
            analogWrite(pwmPin_3, Speed_6);
            break;
          case Speed_7:
            analogWrite(pwmPin_3, Speed_7);
            break;
          default:
            //Serial.println("error");
            break;
        } 
      }else{
        //Serial.println("module number error");
      }
    }else if(input == "start") {
      startReceived = true;
    }else{
      //Serial.println("error");
    }
  }

  if (startReceived == true) {
    pms1.requestRead();
    pms2.requestRead();
    pms3.requestRead();
    

    if(pms1.readUntil(data1)){
      d1 = data1.PM_AE_UG_2_5;
    }

    if(pms2.readUntil(data2)){
      d2 = data2.PM_AE_UG_2_5;
    }
    
    if(pms3.readUntil(data3)){
      d3 = data3.PM_AE_UG_2_5;
    }

    //Serial.println("-----------------------------------------");
    Serial.print(con_data);
    Serial.print(",");
    Serial.print(d1);
    Serial.print(",");
    Serial.print(d2);
    Serial.print(",");
    Serial.print(d3);
    Serial.print(",");
    //Serial.println();
    //Serial.println("----------------------------------------");
    
    startReceived = false;
  }
  
  unsigned long currentTime = millis();
  
  if (currentTime - lastTime >= 1000) {
    noInterrupts();
    
    unsigned long count_1 = tachCounter_1;
    unsigned long count_2 = tachCounter_2;
    unsigned long count_3 = tachCounter_3;
    
    tachCounter_1 = 0;
    tachCounter_2 = 0;
    tachCounter_3 = 0;
    
    interrupts();

    rpm_1 = (count_1 * 60) / 2;
    rpm_2 = (count_2 * 60) / 2;
    rpm_3 = (count_3 * 60) / 2;
    
    if (pwmChanged_1) {
      if(ck_1 >= 3){
        if((rpm_1 >= last_rpm_1 + 390 || rpm_1 <= last_rpm_1 - 390)){
          last_rpm_1 = rpm_1;
          pwmChangedCheck_1 = true;
          con_data = String(fanSpeed1)+ "," + String(fanSpeed2)+ "," + String(fanSpeed3);
        }
        pwmChanged_1 = false; 
      }
    }
    ck_1++;

    if (pwmChanged_2) {
      if(ck_2 >= 3){
        if((rpm_2 >= last_rpm_2 + 390 || rpm_2 <= last_rpm_2 - 390)){
          last_rpm_2 = rpm_2;
          pwmChangedCheck_2 = true;
          con_data = String(fanSpeed1)+ "," + String(fanSpeed2)+ "," + String(fanSpeed3);
        }
        pwmChanged_2 = false; 
      }
    }
    ck_2++;

    if (pwmChanged_3) {
      if(ck_3 >= 3){
        if((rpm_3 >= last_rpm_3 + 390 || rpm_3 <= last_rpm_3 - 390)){
          last_rpm_3 = rpm_3;
          pwmChangedCheck_3 = true;
          con_data = String(fanSpeed1)+ "," + String(fanSpeed2)+ "," + String(fanSpeed3);
        }
        pwmChanged_3 = false; 
      }
    }
    ck_3++;
        
    if(pwmChangedCheck_1){
      //Serial.println("change finish");
      //Serial.print("Fan_1 RPM: ");
      //Serial.println(rpm_1);
      pwmChangedCheck_1 = false;
    }else {
      //Serial.print("Fan_1 RPM: ");
      //Serial.println(rpm_1);
      //Serial.print("last_Fan_1 RPM: ");
      //Serial.println(last_rpm_1);
    }
    
    if(pwmChangedCheck_2){
      //Serial.println("change finish");
      //Serial.print("Fan_2 RPM: ");
      //Serial.println(rpm_2);
      pwmChangedCheck_2 = false;
    }else {
      //Serial.print("Fan_2 RPM: ");
      //Serial.println(rpm_2);
      //Serial.print("last_Fan_2 RPM: ");
      //Serial.println(last_rpm_2);
    }
    
    if(pwmChangedCheck_3){
      //Serial.println("change finish");
      //Serial.print("Fan_3 RPM: ");
      //Serial.println(rpm_3);
      pwmChangedCheck_3 = false;
    }else {
      //Serial.print("Fan_3 RPM: ");
      //Serial.println(rpm_3);
      //Serial.print("last_Fan_3 RPM: ");
      //Serial.println(last_rpm_3);
    }
    
    lastTime = currentTime;
  }
}

void tachCounterISR_1() {
  tachCounter_1++;
}

void tachCounterISR_2() {
  tachCounter_2++;
}

void tachCounterISR_3() {
  tachCounter_3++;
}
