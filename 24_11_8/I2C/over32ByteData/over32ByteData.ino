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

void setup() {
  pinMode(LED, OUTPUT);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void loop() {
  delay(100);
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
  }
}

void requestEvent() {
  char str[] = "1234567890qwertyuiopasdfghjklzxcvbnm,.QERTYUIOPASDFGHJKLZXCVBNM";
  static int chunkIndex = 0;

  if (requestedRegister == READ_REGISTER_1) {
    Wire.write(ledState ? LED_ON : LED_OFF);
  } else if (requestedRegister == READ_REGISTER_2) {
    if (chunkIndex == 0) {
      Wire.write(str, 32);
      chunkIndex = 1;
    } else if (chunkIndex == 1) {
      Wire.write(str + 32, strlen(str) - 32);
      chunkIndex = 0;
    }
  }
}


