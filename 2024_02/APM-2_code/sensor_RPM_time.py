# -*- coding: utf-8 -*-
from datetime import datetime
import serial
import requests
import time

port0 = '/dev/ttyACM0'
port1 = '/dev/ttyACM1'
# NPM
port2 = '/dev/ttyACM2'

url = 'http://114.71.220.59:7579/Mobius/Ksensor_ubicomp'

brate0 = 9600
brate1 = 9600
brate2 = 9600

headers = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': 'SJwrKvZd84v',
    'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

previous_date = None

try:
    ser0 = serial.Serial(port0, baudrate=brate0, timeout=None)
    ser1 = serial.Serial(port1, baudrate=brate1, timeout=None)
    ser2 = serial.Serial(port2, baudrate=brate2, timeout=None)
    print('%s and %s and %s' % (ser2.name, ser0.name, ser1.name))
except Exception as err:
    print("Serial err:", err)

while True:
    try:
        r = requests.get(url + "/RPM/la", headers=headers, timeout=5)
        r.raise_for_status()
        jr = r.json()

        ti_data = jr["m2m:cin"]["con"]

        ti_data = str(ti_data.split(',')[0])

        if ti_data != previous_date:
            previous_date = ti_data
            # debug
            #print("debug_1: ", previous_date)
            #print("debug_2: ", ti_data)

            send_s = 'start'
            send_S = send_s.encode('utf-8')
            ser0.write(send_S)
            ser1.write(send_S)
            ser2.write(send_S)

            if ser0.in_waiting != 0 and ser1.in_waiting != 0 and ser2.in_waiting != 0:
                try:
                    contect2 = ser2.readline()
                    contect0 = ser0.readline()
                    contect1 = ser1.readline()

                    con0_1 = "{},{},{},{}".format(ti_data, contect2[:-2].decode(), contect0[:-2].decode(),
                                                contect1[:-2].decode())

                except Exception as err:
                    print("Serial read err:", err)

                con0_1 = con0_1.replace("\n", "")
                print(con0_1)

                data = "{\n    \"m2m:cin\": {\n        \"con\": \"" + con0_1 + "\"\n    }\n}"

                try:
                    r = requests.request("POST", url + "/data", headers=headers, data=data)
                    r.raise_for_status()
                    jr = r.json()
                except requests.exceptions.RequestException as req_err:
                    print("Request error:", req_err)
                except requests.exceptions.HTTPError as http_err:
                    print("HTTP error:", http_err)
                except Exception as exc:
                    print('There was a problem: %s' % (exc))
                    
    except requests.exceptions.ConnectTimeout as e:
        print("connected time error:", e)
    except requests.exceptions.RequestException as e:
        print("request error:", e)

    except KeyboardInterrupt:
        print("\nProgram interrupted by user")
        break
    except Exception as exc:
        print("Error:", exc)
