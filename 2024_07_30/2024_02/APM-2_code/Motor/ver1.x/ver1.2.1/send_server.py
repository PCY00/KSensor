from datetime import datetime
import serial
import requests
import time

port0 = '/dev/ttyACM0'
url = 'http://114.71.220.59:7579/Mobius/Ksensor_ubicomp_2'
brate0 = 9600

headers = {
  'Accept': 'application/json',
  'X-M2M-RI': '12345',
  'X-M2M-Origin': 'SKsensor_ubicomp_2',
  'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

try:
    ser0 = serial.Serial(port0, baudrate=brate0, timeout=None)
    print('%s' %(ser0.name))
except Exception as err:
    print("Serial err:", err)

while True:
    time.sleep(2)
    if ser0.in_waiting != 0:
        current_time = datetime.now().strftime('%Y%m%d%H%M%S')
        try: 
            contect0 = ser0.readline()
            con0_1 = "{},{},{}".format(current_time, contect0[:-2].decode(), port0)
        except Exception as err:
            print("Serial read err:", err)

        con0_1 = con0_1.replace("\n", "")
        print(con0_1)
        
        data = "{\n    \"m2m:cin\": {\n        \"con\": \"" + con0_1 + "\"\n    }\n}"

        try:
            r = requests.request("POST", url+"/data", headers=headers, data=data)
            r.raise_for_status()
            jr = r.json()
        except requests.exceptions.RequestException as req_err:
            print("Request error:", req_err)
        except requests.exceptions.HTTPError as http_err:
            print("HTTP error:", http_err)
        except Exception as exc:
            print('There was a problem: %s' % (exc))
