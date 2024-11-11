#include <PMS.h>
#include <Wire.h>

#define SLAVE_ADDRESS 0x57

// Register addresses
#define WRITE_REGISTER 0x01
#define READ_REGISTER 0x02
#define START_SIGNAL 0xA0

String con_data = "0,0,0";

const uint8_t pwmPins[] = {8, 9, 10};
const uint8_t tachPins[] = {2, 3, 4};
const uint8_t fanCount = 3;

volatile int tachCounters[fanCount] = {0, 0, 0};
unsigned long previousMillis = 0;
const long interval = 1000;

int targetRPMs[fanCount] = {900, 900, 900};
int currentPWMs[fanCount] = {30, 30, 30};
bool targetReached[fanCount] = {true, true, true};

const int minPWM = 30;

PMS pms1(Serial1);
PMS::DATA data1;

PMS pms2(Serial2);
PMS::DATA data2;

PMS pms3(Serial3);
PMS::DATA data3;

bool startReceived = false;

//uint16_t d1, d2, d3;


void setup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent); // 읽기 요청에 대한 핸들러
  Wire.onReceive(receiveEvent);  // 쓰기 요청에 대한 핸들러

  delay(100);
  
  for (int i = 0; i < fanCount; i++) {
    pinMode(tachPins[i], INPUT_PULLUP);
    pinMode(pwmPins[i], OUTPUT);
    analogWrite(pwmPins[i], minPWM);
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
  //Serial.println("Enter target RPMs for each fan in the format '2000 3000 1500' or 'M1_P1_2000':");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    for (int i = 0; i < fanCount; i++) {
      int realRPM = (tachCounters[i] * 60) / 2;
      tachCounters[i] = 0;

      int error = targetRPMs[i] - realRPM;
      int pwmStep = 0;

      if (abs(error) > 1000) {
        pwmStep = 20;
      } else if (abs(error) > 450) {
        pwmStep = 10;
      } else if (abs(error) > 200) {
        pwmStep = 5;
      } else if (abs(error) > 30) {
        pwmStep = 1;
      }

      if (pwmStep > 0) {
        currentPWMs[i] += (error > 0) ? pwmStep : -pwmStep;
        currentPWMs[i] = constrain(currentPWMs[i], minPWM, 255);
        analogWrite(pwmPins[i], currentPWMs[i]);
      }

      if (abs(error) <= 30 && !targetReached[i]) {
        Serial.print("M1_P");
        Serial.print(i+1);
        Serial.print("_");
        Serial.print(realRPM);
        Serial.println();
        targetReached[i] = true;
      }

      Serial.print("Fan ");
      Serial.print(i + 1);
      Serial.print(" | Target RPM: ");
      Serial.print(targetRPMs[i]);
      Serial.print(" | Real RPM: ");
      Serial.print(realRPM);
      Serial.print(" | PWM: ");
      Serial.println(currentPWMs[i]);
    }
  }
}


void receiveEvent(int howMany) {
    int registerAddress = Wire.read();
    
    if (registerAddress == WRITE_REGISTER) {
        char buffer[32];
        int index = 0;

        while (Wire.available() > 0 && index < sizeof(buffer) - 1) { 
            buffer[index++] = Wire.read();
        }
        buffer[index] = '\0';

        String receivedString = String(buffer);
        Serial.print("received: ");
        Serial.println(receivedString);

        
        int fanIndex = receivedString.substring(4, 5).toInt() - 1;
        int fanSpeed = receivedString.substring(6).toInt();
        if (fanIndex >= 0 && fanIndex < fanCount) {
          targetRPMs[fanIndex] = fanSpeed;
          targetReached[fanIndex] = false;
        }
    }
    else if (registerAddress == READ_REGISTER && Wire.available() > 0) {
        int signal = Wire.read();
        if (signal == START_SIGNAL) {
            startReceived = true;
        }
    }
}

void requestEvent() {
    if (startReceived) {
        startReceived = false;
        
        con_data = (String)targetRPMs[0] + "," + (String)targetRPMs[1] + "," + (String)targetRPMs[2];
        
        int len = con_data.length() + 1;
        char buffer[len];
        con_data.toCharArray(buffer, len);

        Wire.write((uint8_t*)buffer, len);
        
        Serial.println(buffer);
    }
}

// ISR
void tachCounterISR0() {
  tachCounters[0]++;
}

void tachCounterISR1() {
  tachCounters[1]++;
}

void tachCounterISR2() {
  tachCounters[2]++;
}
