const int pwmPin = 9;   // 팬의 PWM 핀을 D9에 연결
const int tachPin = 2;  // 팬의 tach 핀을 D2에 연결
volatile unsigned long tachCounter = 0;
unsigned long lastTime = 0;
unsigned long rpm = 0;
int pwmValue = 128;  // 초기 PWM 값 (50% 듀티 사이클)

void setup() {
  Serial.begin(9600);  // 시리얼 통신 시작
  pinMode(tachPin, INPUT_PULLUP);  // tach 핀을 입력으로 설정
  pinMode(pwmPin, OUTPUT);  // pwm 핀을 출력으로 설정
  attachInterrupt(digitalPinToInterrupt(tachPin), tachCounterISR, FALLING);  // 인터럽트 설정
  analogWrite(pwmPin, pwmValue);  // 초기 PWM 신호 출력
}

void loop() {
  if(Serial.available()){
    String input = Serial.readStringUntil('\n');
    int input_pwm = input.toInt();
    analogWrite(pwmPin, input_pwm);
  }
  unsigned long currentTime = millis();
  
  // 1초마다 RPM 계산
  if (currentTime - lastTime >= 1000) {
    noInterrupts();  // 인터럽트 일시 중지
    unsigned long count = tachCounter;
    tachCounter = 0;  // 카운터 초기화
    interrupts();  // 인터럽트 재개

    rpm = (count * 60) / 2;  // RPM 계산 (팬의 회전은 두 번의 펄스로 하나의 회전이 됨)
    Serial.print("Fan RPM: ");
    Serial.println(rpm);
    
    lastTime = currentTime;
  }
}

void tachCounterISR() {
  tachCounter++;
}
