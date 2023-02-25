#ifndef DHT_h
#define DHT_h

#include <avr/io.h>
#include <Arduino.h>

#define DHT_OK			1
#define DHT_ERR_CHECK	0
#define DHT_ERR_TIMEOUT	-1

volatile typedef enum
{
  DHT11 = 11,  
  DHT21 = 21,
  DHT22 = 22,                                                                         
} DHTtype;

class DHT
{
public:
	DHT(unsigned char pin,unsigned char dhtType);
    unsigned char read();
	float humidity;
	float temperature;
private:
  	unsigned char _pin;
	unsigned char _type;
};
#endif
