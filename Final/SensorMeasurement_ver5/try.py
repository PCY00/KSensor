# -*- coding: utf-8 -*-
from datetime import datetime
import time
import serial

port0 = '/dev/ttyUSB0'
port1 = '/dev/ttyUSB1'

brate0 = 9600
brate1 = 9600

send_s = 'start'
send_S = send_s.encode('utf-8')

try:
  ser0 = serial.Serial(port0, baudrate=brate0, timeout=None)
  ser1 = serial.Serial(port1, baudrate=brate1, timeout=None)
  print('%s and %s' %(ser0.name, ser1.name))
except Exception as err:
  print("Serial err:", err)
    

while True:
  current_time = datetime.now()
  if current_time.minute % 2 == 0 and current_time.second == 0:
    ser0.write(send_S)
    ser1.write(send_S)
    print("send_S")
  time.sleep(1)
  
  if ser0.in_waiting != 0 and ser1.in_waiting != 0:
    time_str = datetime.now().strftime('%Y%m%d%H%M%S')
    try:
      contect0 = ser0.readline()
      contect1 = ser1.readline()
            
      con0_1 = "{},{},{}".format(time_str, contect0[:-2].decode(), contect1[:-2].decode())
      
      print(con0_1)
      print('\n')
    except Exception as err:
      print("Serial read err:", err)

