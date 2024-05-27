#include "DHT_T.h"


DHT::DHT(unsigned char pin,unsigned char type){
  _pin = pin;
  _type = type;
  pinMode(_pin, INPUT);
  digitalWrite(_pin, HIGH);
}


// Return values:
// DHT_OK
// DHT_ERR_CHECK
// DHT_ERR_TIMEOUT
unsigned char DHT::read()
{
	// BUFFER TO RECEIVE
	unsigned char bits[5] = {0,0,0,0,0};
	unsigned char cnt = 7;
	unsigned char idx = 0;
	unsigned char sum;


	// REQUEST SAMPLE
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
	delay(18);
	digitalWrite(_pin, HIGH);
	delayMicroseconds(40);
	pinMode(_pin, INPUT);

	// ACKNOWLEDGE or TIMEOUT
	unsigned int count = 10000;
	while(digitalRead(_pin) == LOW)
		if (count-- == 0) return DHT_ERR_TIMEOUT;

	count = 10000;
	while(digitalRead(_pin) == HIGH)
		if (count-- == 0) return DHT_ERR_TIMEOUT;

	// READ OUTPUT - 40 BITS => 5 BYTES or TIMEOUT
	for (int i=0; i<40; i++)
	{
		count = 10000;
		while(digitalRead(_pin) == LOW)
			if (count-- == 0) return DHT_ERR_TIMEOUT;

		unsigned long t = micros();

		count = 10000;
		while(digitalRead(_pin) == HIGH)
			if (count-- == 0) return DHT_ERR_TIMEOUT;

		if ((micros() - t) > 40) bits[idx] |= (1 << cnt);
		if (cnt == 0)   // next byte?
		{
			cnt = 7;    // restart at MSB
			idx++;      // next byte!
		}
		else cnt--;
	}

	sum = bits[0]+bits[1]+bits[2]+bits[3];
	if(bits[4] != sum) return DHT_ERR_CHECK;
    
	switch(_type)
	{
		case DHT11:
		 humidity = (float)bits[0];
	    temperature = (float)bits[2];
		  break;	
        case DHT21:
	    case DHT22:
	      humidity = (float)((bits[0] << 8)+bits[1])/10;
	      temperature = (float)((bits[2] << 8)+bits[3])/10;
		  break;
		default:
		  break;
	}

	return DHT_OK;
}
