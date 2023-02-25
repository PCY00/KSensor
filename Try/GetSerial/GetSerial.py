import serial

port='/dev/ttyACM1'
brate = 57600

ser = serial.Serial(port, baudrate = brate, timeout = None)       
# timeout = None : readline() method will block until it receives a response from the device

print(ser.name)                                                   #Port Output Used ex) ttyACM1

while True:
	if ser.in_waiting != 0 :                                        # there is any data waiting to be read from the serial port
		contect = ser.readline()
    # Reads data using methods that read data lines from serial ports and retuen them as byte objects
    
		print(contect[:-2].decode())
    #Removes the line break character from the end of the response and convets the byte object into a string
