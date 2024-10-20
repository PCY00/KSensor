#include <PMS.h>

#define err 100

const int pwmPins[] = {8, 9, 10};    // 팬의 PWM 핀들
const int tachPins[] = {2, 3, 21};   // 팬의 TACH 핀들
const int fanCount = 3;              // 팬의 수

volatile int tachCounters[fanCount] = {0, 0, 0};
unsigned long previousMillis = 0;
const long interval = 1000;  // 1초마다 RPM 계산 및 출력

int targetRPMs[7] = {900, 1200, 1650, 2100, 2550, 3060, 3420};
int currentPWMs[fanCount] = {0, 0, 0}; // 현재 PWM 값
int pwmMapping[3][7] = {{0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}};
int rpmMapping[3][7] = {{0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}};

//int realRPM[fanCount] = {0,0,0};
int stat = 0;

int currentTargetRPMs[fanCount] = {0, 0, 0};  // 각 팬의 현재 타겟 RPM을 저장

const int minPWM = 30; // 최소 PWM 값

bool end_ = true;

PMS pms1(Serial1);
PMS::DATA data1;

PMS pms2(Serial2);
PMS::DATA data2;

PMS pms3(Serial3);
PMS::DATA data3;

bool startReceived = false;

uint16_t d1, d2, d3;

void setup() {
  for (int i = 0; i < fanCount; i++) {
    pinMode(tachPins[i], INPUT_PULLUP);  // TACH 핀을 입력으로 설정
    pinMode(pwmPins[i], OUTPUT);         // PWM 핀을 출력으로 설정
    analogWrite(pwmPins[i], 30);          // 초기 PWM 값을 0으로 설정
  }

  attachInterrupt(digitalPinToInterrupt(tachPins[0]), tachCounterISR0, FALLING);
  attachInterrupt(digitalPinToInterrupt(tachPins[1]), tachCounterISR1, FALLING);
  attachInterrupt(digitalPinToInterrupt(tachPins[2]), tachCounterISR2, FALLING);
  
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);

  delay(100);

  pms1.passiveMode();
  pms2.passiveMode();
  pms3.passiveMode();

  // 캘리브레이션 수행
  Serial.println("캘리브레이션 시작...");
  calibrateFans();
}

void loop() { 
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // 새로운 입력 형식 M1_P 처리
    if (input.startsWith("M2_P")) {
      int fanIndex = input.substring(4, 5).toInt() - 1;  // 팬 인덱스 (0부터 시작)
      int fanSpeed = input.substring(6).toInt();  // 입력된 RPM 값
    
      // targetRPMs 배열에서 입력된 RPM과 정확히 일치하는 값을 찾음
      int targetIndex = findExactRPMIndex(fanSpeed);
    
      // 정확히 일치하는 RPM 값이 있을 경우
      if (targetIndex != -1) {
        // 해당 인덱스에 있는 pwmMapping 값을 가져와 팬 속도를 조정
        int pwmValue = pwmMapping[fanIndex][targetIndex];
        
        analogWrite(pwmPins[fanIndex], pwmValue);  // 팬 속도를 조정
        
        // 현재 팬의 타겟 RPM 값을 저장
        currentTargetRPMs[fanIndex] = fanSpeed;
        
      } else {
        // 일치하는 RPM 값이 없는 경우
        Serial.println("No matching RPM value found in targetRPMs.");
      }
    } else if(input == "start") {
      startReceived = true;
    }
  }
  
  // 1초마다 RPM 출력
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // RPM 계산
    int realRPM[fanCount];
    for (int i = 0; i < fanCount; i++) {
      realRPM[i] = (tachCounters[i] * 60) / 2; // TACH 카운터를 RPM으로 변환
      tachCounters[i] = 0; // 카운터 리셋
    }

    // RPM 값 출력
    Serial.print("Current RPMs: ");
    for (int i = 0; i < fanCount; i++) {
      Serial.print(realRPM[i]);
      if (i < fanCount - 1) {
        Serial.print(", ");
      }
    }
    Serial.println();
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

    for (int i = 0; i < fanCount; i++) {
      Serial.print(currentTargetRPMs[i]);
      Serial.print(",");
    }
    Serial.print(d1);
    Serial.print(",");
    Serial.print(d2);
    Serial.print(",");
    Serial.print(d3);
    Serial.println(",");

    startReceived = false;
  }
}

// 모든 팬의 캘리브레이션 함수
void calibrateFans() {
  for(int k = 0; k < 7; k++){
    while(end_){
      // 1초 간격으로 RPM 계산 및 PWM 조정
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
    
        int realRPM_1 = (tachCounters[0] * 60) / 2;
        int realRPM_2 = (tachCounters[1] * 60) / 2; 
        int realRPM_3 = (tachCounters[2] * 60) / 2;
    
        int realRPM[fanCount] = {realRPM_1, realRPM_2, realRPM_3};

        tachCounters[0] = 0;
        tachCounters[1] = 0;
        tachCounters[2] = 0;
    
        int error_1 = targetRPMs[k] - realRPM_1;
        int error_2 = targetRPMs[k] - realRPM_2;
        int error_3 = targetRPMs[k] - realRPM_3;
    
        int error[fanCount] = {error_1, error_2, error_3};
    
        int pwmStep_1 = 0;
        int pwmStep_2 = 0;
        int pwmStep_3 = 0;
    
        int pwmStep[fanCount] = {pwmStep_1, pwmStep_2, pwmStep_3};
    
        for (int i = 0; i < fanCount; i++){
          if (abs(error[i]) > 1000) {
            pwmStep[i] = 50;
          } else if (abs(error[i]) > 450) {
            pwmStep[i] = 10;
          } else if (abs(error[i]) > 200) {
            pwmStep[i] = 5;
          } else if (abs(error[i]) > 30) {
            pwmStep[i] = 1;
          }
        }
    
        if (pwmStep[0] > 0) {
          currentPWMs[0] += (error[0] > 0) ? pwmStep[0] : -pwmStep[0];
          if(currentPWMs[0] > 0){
            currentPWMs[0] = constrain(currentPWMs[0], minPWM, 255);
            analogWrite(pwmPins[0], currentPWMs[0]);
          }
        }
    
        if (pwmStep[1] > 0) {
          currentPWMs[1] += (error[1] > 0) ? pwmStep[1] : -pwmStep[1];
          if(currentPWMs[1] > 0){
            currentPWMs[1] = constrain(currentPWMs[1], minPWM, 255);
            analogWrite(pwmPins[1], currentPWMs[1]);
          }
        }
    
        if (pwmStep[2] > 0) {
          currentPWMs[2] += (error[2] > 0) ? pwmStep[2] : -pwmStep[2];
          if(currentPWMs[2] > 0){
            currentPWMs[2] = constrain(currentPWMs[2], minPWM, 255);
            analogWrite(pwmPins[2], currentPWMs[2]);
          }
        }
        
        // 출력 및 종료 조건 체크
        Serial.print("Current RPMs during calibration: ");
        Serial.print(realRPM[0]);
        Serial.print(", ");
        Serial.print(realRPM[1]);
        Serial.print(", ");
        Serial.println(realRPM[2]);
        if (abs(error[0]) < err && abs(error[1]) < err && abs(error[2]) < err){
          stat++;
        }
        
        if (abs(error[0]) < err && abs(error[1]) < err && abs(error[2]) < err && stat == 4){
          Serial.print("팬 캘리브레이션 완료: ");
          Serial.println(targetRPMs[k]);
          end_ = false; // 캘리브레이션 완료
          stat = 0;
          for (int i = 0; i < fanCount; i++) {
            pwmMapping[i][k] = currentPWMs[i]; // 현재 PWM 값을 매핑 배열에 저장
          }
        }
      }
    }
    end_ = true; // 다음 RPM 캘리브레이션을 위해 초기화
  }
}

int findExactRPMIndex(int rpm) {
  for (int i = 0; i < sizeof(targetRPMs) / sizeof(targetRPMs[0]); i++) {
    if (targetRPMs[i] == rpm) {
      return i;
    }
  }
  return -1; // 일치하는 RPM이 없을 경우
}

void tachCounterISR0() {
  tachCounters[0]++;
}

void tachCounterISR1() {
  tachCounters[1]++;
}

void tachCounterISR2() {
  tachCounters[2]++;
}
