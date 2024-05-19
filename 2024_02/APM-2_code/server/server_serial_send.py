# -*- conding: utf-8 -*-
import serial
import requests
import json
import time
from datetime import datetime

apm_url = 'http://114.71.220.59:7579/Mobius/Ksensor_ubicomp'
url_sejong = 'http://114.71.220.59:7579/Mobius/ubicomp_Ksensor/Sensor1/train'
rpm_url = "http://211.226.18.66:5002/iot/v1/microdust/measure-last"

#DDDOT
port0 = '/dev/ttyACM1'
#CNSWW
port1 = '/dev/ttyACM0'
#NBP1P2P3
port2 = '/dev/ttyACM2'

brate0 = 9600
brate1 = 9600
brate2 = 9600

headers_apm = {
  'Accept': 'application/json',
  'X-M2M-RI': '12345',
  'X-M2M-Origin': 'SJwrKvZd84v',
  'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

headers_sejong = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': 'Ssch20191546',
    'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

# Global variables
time_s, pm_data = None, None
last_sent_data = None

def fetch_data():
    try:
        response = requests.get(rpm_url)
        response.raise_for_status()
        return response.json()
    except requests.HTTPError as http_err:
        print(f"HTTP error occurred: {http_err} - {response.status_code}")
        return None
    except Exception as e:
        print(f"Failed to fetch data: {e}")
        return None

def process_data():
    global last_sent_data
    data = fetch_data()
    if data:
        for item in data.get('data', []):
            if item.get('deviceId') == '0017B2FFFE50087D':
                meastime = item.get('meastime', '')
                pm2_5_a = item.get('pm2.5_a', '')
                print(f"meastime: {meastime}, pm2.5_a: {pm2_5_a}")
                if meastime != last_sent_data or last_sent_data is None:
                    last_sent_data = meastime
                    time_s = meastime
                    pm_data = pm2_5_a
                
                return time_s, pm_data
    else:
        print("No data received")

try:
    ser0 = serial.Serial(port0, baudrate=brate0, timeout=None)
    ser1 = serial.Serial(port1, baudrate=brate1, timeout=None)
    ser2 = serial.Serial(port2, baudrate=brate2, timeout=None)
    print('%s and %s and %s' %(ser2.name, ser0.name, ser1.name))
except Exception as err:
    print("Serial err:", err)
    
while True:
    time_s, pm_data = process_data()
    
    if ser0.in_waiting != 0 and ser1.in_waiting != 0 and ser2.in_waiting != 0:
        try:
            contect2 = ser2.readline()
            contect0 = ser0.readline()
            contect1 = ser1.readline()
            
            con0_1 = "{},{},{},{}".format(time_s, contect2[:-2].decode(), contect0[:-2].decode(), contect1[:-2].decode())
            
        
        except Exception as err:
            print("Serial read err:", err)
        
        con0_1 = con0_1.replace("\n","")
        data_list = con0_1.split(',')
            
        #각도 저장
        bottom, pm1, pm2, pm3 = data_list[2], data_list[3], data_list[4], data_list[5]
            
        # bottom 0,30,60,90 pm1 pm2 pm3 
        if bottom == "0":
            pm1_url = url_sejong + "/param1"
            pm2_url = url_sejong + "/param2"
            pm3_url = url_sejong + "/param3"
        elif bottom == "30":
            pm1_url = url_sejong + "/param4"
            pm2_url = url_sejong + "/param5"
            pm3_url = url_sejong + "/param6"
        elif bottom == "60":
            pm1_url = url_sejong + "/param7"
            pm2_url = url_sejong + "/param8"
            pm3_url = url_sejong + "/param9"
        elif bottom == "90":
            pm1_url = url_sejong + "/param10"
            pm2_url = url_sejong + "/param11"
            pm3_url = url_sejong + "/param12"
            
        new_data_list = data_list[:2] + data_list[6:]

        con0_1_sejong_json = {
            "label": pm_data,
            "data": new_data_list
        }
        
        con0_1_sejong_str = json.dumps(con0_1_sejong_json)
        
        data = "{\n    \"m2m:cin\": {\n        \"con\": \"" + con0_1 + "\"\n    }\n}"
        data_sejong = "{\n    \"m2m:cin\": {\n        \"con\": \"" + con0_1_sejong_str + "\"\n    }\n}"
        
        try:
            r = requests.post(url_sejong, headers=headers_sejong, data=data_sejong)
            r.raise_for_status()
            jr = r.json()
        except requests.exceptions.RequestException as req_err:
            print("Request error:", req_err)
        except requests.exceptions.HTTPError as http_err:
            print("HTTP error:", http_err)
        except Exception as exc:
            print('There was a problem: %s' % (exc))
    
    time.sleep(55)
