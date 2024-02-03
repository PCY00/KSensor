from datetime import datetime, timedelta
import threading
import serial
import requests
import time

port4 = '/dev/ttyACM4'
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
    
except Exception as err:
    print("Serial err:", err)


def calculate_next_run_time():
    now = datetime.now()
    next_run = now.replace(second=0, microsecond=0) + timedelta(minutes=(5 - now.minute % 5))
    if next_run.minute % 5 != 0:
        next_run += timedelta(minutes=(5 - next_run.minute % 5))
    return next_run

def print_current_time():
    global next_run

    current_time = datetime.now()
    send_s = 'start'
    send_S = send_s.encode('utf-8')
    ser4.write(send_S)

    next_run = calculate_next_run_time()
    delay = (next_run - datetime.now()).total_seconds()
    threading.Timer(max(0, delay), print_current_time).start()

def print_two():
    while True:
        #time.sleep(2)

        if ser4.in_waiting != 0:
            current_time = datetime.now().strftime('%Y%m%d%H%M%S')
            try:
                contect4 = ser4.readline()
            
                con4_1 = "{},{}".format(current_time, contect4[:-2].decode())
            
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
        
next_run = calculate_next_run_time()
threading.Thread(target=print_current_time).start()
threading.Thread(target=print_two).start()
