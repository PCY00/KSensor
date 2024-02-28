#ifndef RPM_V1_H
#define RPM_V1_H
#include <SoftwareSerial.h>

#define baudRate 19200
#define stx 0x02
#define Checksum 0xFF

//request
#define request_cmd 0x10
#define request_size_1 0x00
#define request_size_2 0x00

//response
#define response_cmd 0x11
#define response_size_1 0x18
#define response_size_2 0x00

class RPM_V1 {
public:
	RPM_V1(uint8_t rx, uint8_t tx);
	void Value_Query();
	int value_Return();
	String show_data();
  int get_data(int num);
	bool runEvery(unsigned long interval);

private:
  SoftwareSerial rpm;
	uint16_t Data[12];
};

#endif
