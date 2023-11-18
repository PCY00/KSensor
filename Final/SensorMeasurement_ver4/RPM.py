import serial
import requests
from datetime import datetime

port4 = '/dev/ttyACM3'
url = 'http://114.71.220.59:7579/Mobius/Ksensor_ubicomp'
brate4 = 19200
headers = {
  'Accept': 'application/json',
  'X-M2M-RI': '12345',
  'X-M2M-Origin': 'SJwrKvZd84v',
  'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

try:
    ser4 = serial.Serial(port4, baudrate=brate4, timeout=None)
    print('%s' %(ser4.name))
    
    # "start" 문자열을 시리얼 포트로 전송
    ser4.write(b'start\n')
    
except Exception as err:
    print("Serial err:", err)

while True:
    if ser4.in_waiting != 0:
        time = datetime.now().strftime('%Y%m%d%H%M%S')
        try:
            contect4 = ser4.readline()
            
            con4_1 = "{},{}".format(time, contect4[:-2].decode())
            
        except Exception as err:
            print("Serial read err:", err)

        con4_1 = con4_1.replace("\n", "")
        print(con4_1)
        
        data = "{\n    \"m2m:cin\": {\n        \"con\": \"" + con4_1 + "\"\n    }\n}"

        try:
            r = requests.request("POST", url+"/RPM", headers=headers, data=data)
            r.raise_for_status()
            jr = r.json()
        except requests.exceptions.RequestException as req_err:
            print("Request error:", req_err)
        except requests.exceptions.HTTPError as http_err:
            print("HTTP error:", http_err)
        except Exception as exc:
            print('There was a problem: %s' % (exc))
