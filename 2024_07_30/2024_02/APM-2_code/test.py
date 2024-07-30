# -*- coding: utf-8 -*-
import sys
from serial import Serial
import requests
import time
import json

apm2_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/data'

apm2_headers = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': 'SKsensor_ubicomp_2',
    'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

def main():
    if len(sys.argv) != 2:
        print("Usage: python_script.py <data>")
        sys.exit(1)

    data = sys.argv[1]

    # Set up serial connection
    try:
        ser0 = Serial('/dev/ttyACM0', 9600, timeout=1)
        ser1 = Serial('/dev/ttyACM1', 9600, timeout=1)
        ser2 = Serial('/dev/ttyACM2', 9600, timeout=1)
        ser3 = Serial('/dev/ttyACM3', 9600, timeout=1)
        ser4 = Serial('/dev/ttyACM4', 9600, timeout=1)
        
        time.sleep(2)  # Wait for the serial connection to initialize

        if True:
            # Send data to Arduino
            ser0.write(data.encode('utf-8'))
            ser1.write(data.encode('utf-8'))
            ser2.write(data.encode('utf-8'))
            ser3.write(data.encode('utf-8'))
            ser4.write(data.encode('utf-8'))
            time.sleep(5)

            if ser0.readable():
                contect0 = ser0.readline().decode('utf-8').strip()
            if ser1.readable():
                contect1 = ser1.readline().decode('utf-8').strip()
            if ser2.readable():
                contect2 = ser2.readline().decode('utf-8').strip()
            if ser3.readable():
                contect3 = ser3.readline().decode('utf-8').strip()
            if ser4.readable():
                contect4 = ser4.readline().decode('utf-8').strip()
                
            con0_1 = "{}{}{}{},{}".format(contect0,contect1,contect2,contect3,contect4)
            con0_1 = con0_1.replace("\n", "")

            data_apm = json.dumps({
                "m2m:cin": {
                    "con": con0_1
                }
            })
            try:
                r_apm2 = requests.post(apm2_url, headers=apm2_headers, data=data_apm)
                r_apm2.raise_for_status()
            except requests.exceptions.RequestException as req_err:
                print("APM Request error:", req_err)
            except requests.exceptions.HTTPError as http_err:
                print("APM HTTP error:", http_err)
            except Exception as exc:
                print('APM There was a problem: {}'.format(exc))
            
    except Serial.SerialException as e:
        print(f"Error opening or communicating with serial port: {e}")

if __name__ == "__main__":
    main()

