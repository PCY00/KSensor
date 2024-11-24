#include <Wire.h>

#define LED           13
#define SLAVE_ADDRESS 0x57

// Register addresses
#define WRITE_REGISTER 0x01
#define READ_REGISTER_1 0x02
#define READ_REGISTER_2 0x04

// LED control
#define LED_ON         0xAC
#define LED_OFF        0x1F

bool ledState = false;
int requestedRegister = 0;

char CO_data[60];    //Hardware-designed co data archiving
float coPPM;
bool checking = false;
unsigned long prevTime1 = 0;
unsigned long prevTime2 = 0;
const unsigned long delay1 = 1000;
const unsigned long delay2 = 100;

bool dataRequested = false;
bool dataProcessed = false;


float value_convert(String value);
void customDtostrf(float value, int width, int precision, char* output);

void setup() {
  pinMode(LED, OUTPUT);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  Serial1.begin(9600);        //CO
}

void loop() {
 if(checking == true){
  static int i = 0;
  static bool collectingData = false;

  if (!dataRequested) {
    Serial1.write('\r');
    prevTime1 = millis();
    dataRequested = true;
    return;
  }
  
  if (dataRequested && !collectingData && (millis() - prevTime1 >= delay1)) {
    collectingData = true;
    i = 0;
  }

  if (collectingData && Serial1.available()) {
    CO_data[i++] = Serial1.read();
  }

  if (collectingData && (millis() - prevTime1 >= delay1 + delay2)) {
    collectingData = false;
    dataProcessed = true;
  }

  if (dataProcessed) {
    String str_co_data = "";
    for (int j = 12; j < 18; j++) {
      str_co_data += CO_data[j];
    }

    coPPM = value_convert(str_co_data);

    Serial.println(coPPM, 3);

    dataRequested = false;
    dataProcessed = false;
    checking = false;
  }
 }
}

void receiveEvent(int howMany) {
  int registerAddress = Wire.read();
  Serial.println(registerAddress);
  if (registerAddress == WRITE_REGISTER) {
    int value = Wire.read();
    if (value == LED_ON) {
      digitalWrite(LED, HIGH);
      ledState = true;
    } else if (value == LED_OFF) {
      digitalWrite(LED, LOW);
      ledState = false;
    }
  }
  else if (registerAddress == READ_REGISTER_1 || registerAddress == READ_REGISTER_2) {
    requestedRegister = registerAddress;
    checking = true;
  }
}

void requestEvent() {
  if (requestedRegister == READ_REGISTER_1) {
    Wire.write(ledState ? LED_ON : LED_OFF);
  } else if (requestedRegister == READ_REGISTER_2) {

    char str[10];
    customDtostrf(coPPM, 6, 3, str);

    Wire.write(str, strlen(str)); 
    Wire.write(0xFF);
  }
}

float value_convert(String value){
  int start = value.indexOf(',') + 1;
  int end = value.indexOf(',', start);

  float con_ppm = 0.0;
  String con_value = value.substring(start, end);
  int convalue = con_value.toInt();
  con_ppm = (float)convalue / 1000;

  return con_ppm;
}

void customDtostrf(float value, int width, int precision, char* output) {
    int whole = (int)value;                       // 정수 부분
    float fractional = value - whole;            // 소수 부분
    if (fractional < 0) fractional *= -1;        // 음수 처리

    int fraction = (int)(fractional * pow(10, precision)); // 소수점 자리수 계산

    // 문자열로 변환
    snprintf(output, width + precision + 2, "%*d.%0*d", width - precision - 1, whole, precision, fraction);
}

