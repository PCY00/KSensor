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
char NO2_data[60];    //Hardware-designed co data archiving
char SO2_data[60];    //Hardware-designed co data archiving

float coPPM = 0.0f;   // CO
float no2PPM = 0.0f;  // NO2
float so2PPM = 0.0f;  // SO2

bool checking = false;
unsigned long lastRequestTime1 = 0;
unsigned long lastRequestTime2 = 0;
unsigned long lastRequestTime3 = 0;
const unsigned long requestInterval = 1000;

bool dataRequested = false;
bool dataProcessed = false;


float value_convert(String value);
void customDtostrf(float value, int width, int precision, char* output);
bool isValidData(String data);

void setup() {
  pinMode(LED, OUTPUT);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  Serial1.begin(9600);        //CO
  Serial2.begin(9600);        //NO2
  Serial3.begin(9600);        //SO2
  delay(100);
}

void loop() {
  static int i = 0, k = 0, l = 0;

  // CO
  if (millis() - lastRequestTime1 >= requestInterval) {
    Serial1.write('\r');
    lastRequestTime1 = millis();
    i = 0;
    delay(150);
    while (Serial1.available()) {
      CO_data[i++] = Serial1.read();
    }
    CO_data[i] = '\0';
    String dataStr = String(CO_data);
    if (isValidData(dataStr)) {
      float value = value_convert(dataStr);
      if (value > 10.0 || value < 0) {
        coPPM = coPPM;
      } else {
        coPPM = (coPPM + value) / 2.0;
      }
    }
  }

  // NO2
  if (millis() - lastRequestTime2 >= requestInterval) {
    Serial2.write('\r');
    lastRequestTime2 = millis();
    k = 0;
    delay(150);
    while (Serial2.available()) {
      NO2_data[k++] = Serial2.read();
    }
    NO2_data[k] = '\0';
    String dataStr = String(NO2_data);
    if (isValidData(dataStr)) {
      float value = value_convert(dataStr);
      if (value > 10.0 || value < 0) {
        no2PPM = no2PPM;
      } else {
        no2PPM = (no2PPM + value) / 2.0;
      }
    }
  }

  // SO2
  if (millis() - lastRequestTime3 >= requestInterval) {
    Serial3.write('\r');
    lastRequestTime3 = millis();
    l = 0;
    delay(150);
    while (Serial3.available()) {
      SO2_data[l++] = Serial3.read();
    }
    SO2_data[l] = '\0';

    String dataStr = String(SO2_data);
    if (isValidData(dataStr)) {
      float value = value_convert(dataStr);
      if (value > 10.0 || value < 0) {
        so2PPM = so2PPM;
      } else {
        so2PPM = (so2PPM + value) / 2.0; // 이전 값과 평균 처리
      }
    }
    //Serial.print("CO: ");
    //Serial.print(coPPM, 3);
    //Serial.print(" ppm, NO2: ");
    //Serial.print(no2PPM, 3);
    //Serial.print(" ppm, SO2: ");
    //Serial.println(so2PPM, 3);    
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
    char coStr[10], no2Str[10], so2Str[10];
    customDtostrf(coPPM, 6, 3, coStr);
    customDtostrf(no2PPM, 6, 3, no2Str);
    customDtostrf(so2PPM, 6, 3, so2Str);
    
    char combinedStr[40];
    snprintf(combinedStr, sizeof(combinedStr), "%s,%s,%s", coStr, no2Str, so2Str);


    Wire.write(combinedStr, strlen(combinedStr));
    Wire.write(0xFF);
  }
}


float value_convert(String value) {

  int start = value.indexOf(',') + 1;
  int end = value.indexOf(',', start);

  if (start == -1 || end == -1) {
    return 0.0;
  }

  String extractedValue = value.substring(start, end);
  int rawValue = extractedValue.toInt();
  return (float)rawValue / 1000.0; // ppm 단위로 변환
}

bool isValidData(String data) {
  int commaCount = 0;
  for (int i = 0; i < data.length(); i++) {
    if (data[i] == ',') commaCount++;
  }
  return (commaCount == 6);
}

void customDtostrf(float value, int width, int precision, char* output) {
    int whole = (int)value;
    float fractional = value - whole;
    if (fractional < 0) fractional *= -1;

    int fraction = (int)(fractional * pow(10, precision));

    snprintf(output, width + precision + 2, "%*d.%0*d", width - precision - 1, whole, precision, fraction);
}

