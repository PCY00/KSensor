import serial

port='/dev/ttyACM1'
brate = 57600

ser = serial.Serial(port, baudrate = brate, timeout = None)
print(ser.name)

f = open('GetSerial.txt','a') # a : Add-Mode -> used to add new content at the end of a file

while True:
	if ser.in_waiting != 0 :
		contect = ser.readline()
		f.writelines(contect)
