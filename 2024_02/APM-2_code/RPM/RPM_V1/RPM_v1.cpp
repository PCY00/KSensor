#include <Arduino.h>
#include <SoftwareSerial.h>
#include "RPM_v1.h"

RPM_V1::RPM_V1(uint8_t rx, uint8_t tx) : rpm(rx, tx) {
	rpm.begin(baudRate);
}
void RPM_V1::Value_Query() {
	uint8_t chsum = (Checksum - (request_cmd + request_size_1 + request_size_2));
	//Serial.println("��û");
	rpm.write((uint8_t)stx);
	rpm.write((uint8_t)request_cmd);
	rpm.write((uint8_t)request_size_1);
	rpm.write((uint8_t)request_size_2);
	rpm.write(chsum);
	//Serial.println("��û�� ");

	//�� ��û�ϸ鼭 �ʱ�ȭ
	for (uint8_t i = 0; i < 12; i++) {
		Data[i] = 0x00;
	}
	//Serial.println("�ʱ�ȭ");
}

int RPM_V1::value_Return() {
    if (rpm.available() >= 29) {
        //Serial.println("�б� ����");
        uint8_t STX = rpm.read();
        uint8_t CMD = rpm.read();
        uint8_t SIZE_1 = rpm.read();
        uint8_t SIZE_2 = rpm.read();

        if (STX != stx || CMD != response_cmd) {
            return -1;
            //Serial.println("stx and cmd error");
        }

        // (lsb) + (msb)<<8
        for (uint8_t i = 0; i < 12; i++) {
            Data[i] = (rpm.read() | (rpm.read() << 8));
        }

        uint8_t chsum = rpm.read();

        uint16_t cal_check = (Checksum - CMD - response_size_1 - response_size_2);
        for (uint8_t i = 0; i < 12; i++) {
            cal_check -= Data[i];
        }
        if (chsum != cal_check) {
            return -2;
            //Serial.println("checksum error");
        }
        return 0;
    }
    //data loss
    return -3;
}

String RPM_V1::show_data() {
    char save_data[50];
    /*
    for (uint8_t i = 0; i < 12; i++) {
        Serial.print(Data[i]);
        Serial.print(",");
    }
    */
    //NULL���� �߰��� string ����
    sprintf(save_data, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6], Data[7], Data[8], Data[9], Data[10], Data[11] );

    return save_data;
}

bool RPM_V1::runEvery(unsigned long interval) {
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        return true;
    }
    return false;
}
