import serial
import requests
from datetime import datetime

port0 = '/dev/ttyACM1'
port1 = '/dev/ttyACM0'
#NPM
port2 = '/dev/ttyACM2'

url = 'http://114.71.220.59:7579/Mobius/Ksensor_project'

brate0 = 9600
brate1 = 9600
brate2 = 9600


headers = {
  'Accept': 'application/json',
  'X-M2M-RI': '12345',
  'X-M2M-Origin': 'S9arsI6_8fm',
  'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}


try:
    ser0 = serial.Serial(port0, baudrate = brate0, timeout = None)
    ser1 = serial.Serial(port1, baudrate = brate1, timeout = None)
    ser2 = serial.Serial(port2, baudrate = brate2, timeout = None)
    print('%s and %s and %s' %(ser2.name, ser0.name, ser1.name))
    #print('Time, NPM, Dust1, Dust2, Dust3, Temp, Humi, Ozone, CO, NO2, SO2, WindDirection, WindSpeed\n') 
except Exception as err:
    print("Serial err:", err)

while True:
    if ser0.in_waiting != 0 and ser1.in_waiting != 0 and ser2.in_waiting != 0:
        time = datetime.now().strftime('%Y%m%d%H%M%S')
        try:
            contect2 = ser2.readline()
            contect0 = ser0.readline()
            contect1 = ser1.readline()
            
            # print('%s, %s, %s' %(time, contect0[:-2].decode(), contect1[:-2].decode()))
            con0_1 = "{},{},{},{}".format(time, contect2[:-2].decode(), contect0[:-2].decode(), contect1[:-2].decode())
            
        except Exception as err:
            print("Serial read err:", err)
            # con0_1 = "20230824205835,0,2,4,4,22.50,48.20,0.020,0.442,0.000,0.000,0.00,0\n"


        con0_1 = con0_1.replace("\n", "")
        print(con0_1)
        
        data = "{\n    \"m2m:cin\": {\n        \"con\": \"" + con0_1 + "\"\n    }\n}"

        r = requests.request("POST", url+"/Data", headers=headers, data=data)

        try:
            r.raise_for_status()
            jr = r.json()

        except Exception as exc:
            print('There was a problem: %s' % (exc))
