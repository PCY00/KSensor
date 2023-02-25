import serial
from datetime import datetime

port0 = '/dev/ttyACM0'
port1 = '/dev/ttyACM1'

brate0 = 9600
brate1 = 57600

ser0 = serial.Serial(port0, baudrate = brate0, timeout = None)
ser1 = serial.Serial(port1, baudrate = brate1, timeout = None)

print('%s and %s' % (ser0.name, ser1.name))
print('Time, Dust1, Dust2, Dust3, Temp, Humi, Ozone, SO2, NO2, CO, WindSpeed, WindDirection\n') 

while True:
	now = datetime.now()
	time = now.strftime('%Y%m%d%H%M%S')

	if ser0.in_waiting != 0 and ser1.in_waiting != 0 :
		contect0 = ser0.readline()
		contect1 = ser1.readline()
		print('%s, %s, %s' %(time, contect0[:-2].decode(), contect1[:-2].decode()))
