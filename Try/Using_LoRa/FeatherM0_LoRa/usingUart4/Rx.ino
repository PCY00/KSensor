//수신부
#include <Arduino.h>        // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function

//co
Uart Serial2(&sercom5, A5, 6, SERCOM_RX_PAD_0, UART_TX_PAD_2);
void SERCOM5_Handler()
{
    Serial2.IrqHandler();
}

//no2
Uart Serial3(&sercom1, 12, 10, SERCOM_RX_PAD_3, UART_TX_PAD_2);
void SERCOM1_Handler()
{
    Serial3.IrqHandler();
}
//so2
Uart Serial4(&sercom0, A4, A3, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM0_Handler()
{
    Serial4.IrqHandler();
}


void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  Serial4.begin(9600);

  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(12, PIO_SERCOM);

  pinPeripheral(1, PIO_SERCOM_ALT);
  pinPeripheral(0, PIO_SERCOM_ALT);
  
  pinPeripheral(6, PIO_SERCOM);
  pinPeripheral(A5, PIO_SERCOM_ALT);

  pinPeripheral(17ul, PIO_SERCOM_ALT);
  pinPeripheral(18ul, PIO_SERCOM_ALT);

}

void loop(){
  String str = "";
  
  while(Serial1.available()){
    char received = Serial1.read();
    str += received;
  }
  Serial.print("serial1: ");
  Serial.println(str);
  
  while(Serial2.available()){
    char received = Serial2.read();
    str += received;
  }
  Serial.print("serial1+2: ");
  Serial.println(str);

  while(Serial3.available()){
    char received = Serial3.read();
    str += received;
  }
  Serial.print("serial1+2+3: ");
  Serial.println(str);

  while(Serial4.available()){
    char received = Serial4.read();
    str += received;
  }
  Serial.print("serial1+2+3+4: ");
  Serial.println(str);
  
  delay(1000);
}

// 보내주는 쪽하고 싱크를 맞춰라 delay이도
