# -*- coding: utf-8 -*-
import requests
import json
import time
from serial import Serial

apm_url = 'http://114.71.220.59:7579/Mobius/Ksensor_ubicomp'
url_sejong = 'http://114.71.220.59:7579/Mobius/ubicomp_Ksensor/Sensor1/train'
rpm_url = "http://211.226.18.66:5002/iot/v1/microdust/measure-last"

headers_apm = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': 'SJwrKvZd84v',
    'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

headers_sejong = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': 'So7KqXYRv1JQ',
    'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

# Serial port definitions
port0 = '/dev/ttyACM2'
port1 = '/dev/ttyACM0'
port2 = '/dev/ttyACM3'

brate0 = 9600
brate1 = 9600
brate2 = 9600

# Initialize serial connections
try:
    ser0 = Serial(port0, baudrate=brate0, timeout=None)
    ser1 = Serial(port1, baudrate=brate1, timeout=None)
    ser2 = Serial(port2, baudrate=brate2, timeout=None)
    print('%s and %s and %s' % (ser2.name, ser0.name, ser1.name))
except Exception as err:
    ser0, ser1, ser2 = None, None, None
    print("Serial error:", err)

# Global variables
time_s, pm_data = None, None
last_sent_data = None

def fetch_data():
    try:
        response = requests.get(rpm_url)
        response.raise_for_status()
        return response.json()
    except requests.HTTPError as http_err:
        print("HTTP error occurred: {} - {}".format(http_err, response.status_code))
        return None
    except Exception as e:
        print("Failed to fetch data: {}".format(e))
        return None

def process_data():
    global last_sent_data, ser0, ser1, ser2
    time_s, pm_data = None, None  #
    data = fetch_data()
    if data:
        for item in data.get('data', []):
            if item.get('deviceId') == '0017B2FFFE50087D':
                meastime = item.get('meastime', '')
                print("meastime: ", meastime)
                pm2_5_a = item.get('pm2.5_a', '')
                if meastime != last_sent_data or last_sent_data is None:
                    last_sent_data = meastime
                    time_s = meastime
                    pm_data = pm2_5_a
                
                send_s = 'start'
                send_S = send_s.encode('utf-8')
                
                if ser0:
                    ser0.write(send_S)
                if ser1:
                    ser1.write(send_S)
                if ser2:
                    ser2.write(send_S)
                
                time.sleep(2)
                return time_s, pm_data
    else:
        print("No data received")
    return time_s, pm_data  #

while True:
    url_sejong_uni = ""
    
    time_s, pm_data = process_data()
    print("time_s: ", time_s)
    
    if ser0 and ser1 and ser2 and ser0.in_waiting and ser1.in_waiting and ser2.in_waiting:
        try:
            contect2 = ser2.readline()
            contect0 = ser0.readline()
            contect1 = ser1.readline()
            
            con0_1 = "{},{},{},{}".format(time_s, contect2[:-2].decode(), contect0[:-2].decode(), contect1[:-2].decode())
            print(con0_1)

        except Exception as err:
            print("Serial read error:", err)
        
        con0_1 = con0_1.replace("\n", "")
        data_list = con0_1.split(',')
            
        # Store angles and pm values
        bottom, pm1 = data_list[2], data_list[3]
        print("bottom: ", data_list[2])
            
        if bottom == "0":
            if pm1 == "1":
                url_sejong_uni = url_sejong + "/param1"
            elif pm1 == "2":
                url_sejong_uni = url_sejong + "/param2"
            elif pm1 == "3":
                url_sejong_uni = url_sejong + "/param3"
        elif bottom == "30":
            if pm1 == "1":
                url_sejong_uni = url_sejong + "/param4"
            elif pm1 == "2":
                url_sejong_uni = url_sejong + "/param5"
            elif pm1 == "3":
                url_sejong_uni = url_sejong + "/param6"
        elif bottom == "60":
            if pm1 == "1":
                url_sejong_uni = url_sejong + "/param7"
            elif pm1 == "2":
                url_sejong_uni = url_sejong + "/param8"
            elif pm1 == "3":
                url_sejong_uni = url_sejong + "/param9"
        elif bottom == "90":
            if pm1 == "1":
                url_sejong_uni = url_sejong + "/param10"
            elif pm1 == "2":
                url_sejong_uni = url_sejong + "/param11"
            elif pm1 == "3":
                url_sejong_uni = url_sejong + "/param12"
                
        print("url_sejong_uni: ", url_sejong_uni)
            
        new_data_list = data_list[:2] + data_list[6:]

        con0_1_sejong_json = {
            "label": pm_data,
            "data": new_data_list
        }
        
        con0_1_sejong_str = json.dumps(con0_1_sejong_json)
        
        data = "{\n    \"m2m:cin\": {\n        \"con\": \"" + con0_1 + "\"\n    }\n}"
        data_sejong = "{\n    \"m2m:cin\": {\n        \"con\": \"" + con0_1_sejong_str + "\"\n    }\n}"
        
        try:
            r = requests.post(url_sejong_uni, headers=headers_sejong, data=data_sejong)
            r.raise_for_status()
            jr = r.json()
        except requests.exceptions.RequestException as req_err:
            print("Request error:", req_err)
        except requests.exceptions.HTTPError as http_err:
            print("HTTP error:", http_err)
        except Exception as exc:
            print('There was a problem: {}'.format(exc))
    
    time.sleep(55)
