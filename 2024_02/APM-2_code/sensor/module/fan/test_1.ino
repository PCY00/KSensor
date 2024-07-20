#include <PMS.h>

const int pwmPin = 9;   // 팬의 PWM 핀을 D9에 연결
const int tachPin = 2;  // 팬의 tach 핀을 D2에 연결
volatile unsigned long tachCounter = 0;
unsigned long lastTime = 0;
unsigned long last_rpm = 0;
unsigned long rpm = 0;
int pwmValue = 0;  // 초기 PWM 값 (50% 듀티 사이클)

int ck = 0;

PMS pms1(Serial1);
PMS::DATA data1;

bool startReceived = false;
bool pwmChanged = false;
bool pwmChangedCheck = false;

uint16_t d1;

void setup() {
  Serial.begin(9600);  // 시리얼 통신 시작
  Serial1.begin(9600);
  pinMode(tachPin, INPUT_PULLUP);  // tach 핀을 입력으로 설정
  pinMode(pwmPin, OUTPUT);  // pwm 핀을 출력으로 설정
  attachInterrupt(digitalPinToInterrupt(tachPin), tachCounterISR, FALLING);  // 인터럽트 설정
  analogWrite(pwmPin, pwmValue);  // 초기 PWM 신호 출력

  pms1.passiveMode();
}

void loop() {
  if(Serial.available()){
    String input = Serial.readStringUntil('\n');
    if(input == "start"){
        startReceived = true;
    } else {
      int input_pwm = input.toInt();
      analogWrite(pwmPin, input_pwm);
      ck = 0;
      pwmChanged = true;
    }
  }

  unsigned long currentTime = millis();
  // 1초마다 RPM 계산
  if (currentTime - lastTime >= 1000) {
    noInterrupts();  // 인터럽트 일시 중지
    unsigned long count = tachCounter;
    tachCounter = 0;  // 카운터 초기화
    interrupts();  // 인터럽트 재개

    rpm = (count * 60) / 2;  // RPM 계산 (팬의 회전은 두 번의 펄스로 하나의 회전이 됨)
    if (pwmChanged) {
      if(ck >= 3){
        if((rpm >= last_rpm + 390 || rpm <= last_rpm - 390)){
          last_rpm = rpm;
          pwmChangedCheck = true;
        }
        pwmChanged = false; 
      }
    }
    ck++;
    if(pwmChangedCheck){
      Serial.println("change finish");
      Serial.print("Fan RPM: ");
      Serial.println(rpm);
      pwmChangedCheck = false;
    }else {
      Serial.print("Fan RPM: ");
      Serial.println(rpm);
      Serial.print("last_Fan RPM: ");
      Serial.println(last_rpm);
    }
    lastTime = currentTime;
  }

  if (startReceived == true) {
    pms1.requestRead();

    if(pms1.readUntil(data1)){
      d1 = data1.PM_AE_UG_2_5;
    }
    
    Serial.println(d1);
    
    startReceived = false;
  }
}

void tachCounterISR() {
  tachCounter++;
}
