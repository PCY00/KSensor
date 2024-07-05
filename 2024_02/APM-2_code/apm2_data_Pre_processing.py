# -*- coding: utf-8 -*-
from datetime import datetime
import serial
import requests
import time
import json

# Module1,2,3
port0 = '/dev/ttyACM0'
port1 = '/dev/ttyACM1'
port2 = '/dev/ttyACM2'
# NPM
port3 = '/dev/ttyACM3'
# Sensor
port4 = '/dev/ttyACM4'
# Motor
port5 = '/dev/ttyACM5'

apm2_url = 'http://114.71.220.59:7579/Mobius/Ksensor_ubicomp2'
rpm_url = "http://211.226.18.66:5002/iot/v1/microdust/measure-last"

brate0 = 9600
brate1 = 9600
brate2 = 9600
brate3 = 9600
brate4 = 9600
brate5 = 9600

apm2_headers = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': '?',
    'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

try:
    ser0 = serial.Serial(port0, baudrate=brate0, timeout=None)
    ser1 = serial.Serial(port1, baudrate=brate1, timeout=None)
    ser2 = serial.Serial(port2, baudrate=brate2, timeout=None)
    ser3 = serial.Serial(port3, baudrate=brate3, timeout=None)
    ser4 = serial.Serial(port4, baudrate=brate4, timeout=None)
    ser5 = serial.Serial(port5, baudrate=brate5, timeout=None)
    print('%s and %s and %s and %s and %s and %s' % (ser0.name, ser1.name,ser2.name, ser3.name,ser4.name, ser5.name))
except Exception as err:
    print("Serial err:", err)
    
# Global variables
time_s, pm_data = "", ""
last_sent_data, pm_data_save = "", ""

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
    global last_sent_data, pm_data_save, ser0, ser1, ser2, time_s, pm_data
    data = fetch_data()
    if data:
        for item in data.get('data', []):
            if item.get('deviceId') == '0017B2FFFE50087D':
                meastime = item.get('meastime', '')
                pm2_5_a = item.get('pm2.5_a', '')
                if meastime != last_sent_data or not last_sent_data:
                    last_sent_data = meastime
                    time_s = meastime
                    pm_data_save = pm2_5_a
                    pm_data = pm_data_save
                
                send_s = 'start'
                send_S = send_s.encode('utf-8')
                
                if ser0:
                    ser0.write(send_S)
                if ser1:
                    ser1.write(send_S)
                if ser2:
                    ser2.write(send_S)
                if ser3:
                    ser3.write(send_S)
                if ser4:
                    ser4.write(send_S)
                if ser5:
                    ser5.write(send_S)
                
                time.sleep(2)
                return time_s, pm_data
    else:
        print("No data received")
    return time_s, pm_data

while True:
    time_s, pm_data = process_data()
    
    if ser0 and ser1 and ser2 and ser3 and ser4 and ser5 and ser0.in_waiting and ser1.in_waiting and ser2.in_waiting and ser3.in_waiting and ser4.in_waiting and ser5.in_waiting:
        try:
            contect0 = ser0.readline().decode().strip()
            contect1 = ser1.readline().decode().strip()
            contect2 = ser2.readline().decode().strip()
            contect3 = ser3.readline().decode().strip()
            contect4 = ser4.readline().decode().strip()
            contect5 = ser5.readline().decode().strip()
            
            con0_1 = "{},{},{},{},{},{},{},{},{}".format(time_s, contect0, contect1, contect2, contect3, contect4, contect5, pm_data, pm_data)
            
            #data 정리
            data_list = con0_1.split(',')
            
            # 원래 데이터 순서 (0부터 시작하는 인덱스)
            original_order = [
                "datetime", "pwm1_1", "pwm1_2", "pwm1_3", "pm1_1", "pm1_2", "pm1_3", 
                "pwm2_1", "pwm2_2", "pwm2_3", "pm2_1", "pm2_2", "pm2_3", 
                "pwm3_1", "pwm3_2", "pwm3_3", "pm3_1", "pm3_2", "pm3_3", 
                "npm", "temp", "humi", "o3", "co", "no2", "so2", 
                "wind_d", "wind_s", "Bottom1", "Bottom2", "Bottom3", 
                "rpm보정 전", "rpm 보정후"
            ]
            
            # 원하는 순서 (0부터 시작하는 인덱스)
            desired_order = [
                "datetime", "rpm보정 전", "rpm 보정후", 
                "pwm1_1", "pwm1_2", "pwm1_3", 
                "pwm2_1", "pwm2_2", "pwm2_3", 
                "pwm3_1", "pwm3_2", "pwm3_3", 
                "Bottom1", "Bottom2", "Bottom3", 
                "npm", "pm1_1", "pm1_2", "pm1_3", 
                "pm2_1", "pm2_2", "pm2_3", 
                "pm3_1", "pm3_2", "pm3_3", 
                "temp", "humi", "o3", "co", 
                "no2", "so2", "wind_d", "wind_s"
            ]
            
            # 원래 순서에서 원하는 순서로 데이터 재배치
            index_map = {original_order[i]: i for i in range(len(original_order))}
            reordered_data = [data_list[index_map[col]] for col in desired_order]

            # 재조합된 데이터 출력
            con0_1 = ','.join(reordered_data)
            print(con0_1)


        except Exception as err:
            print("Serial read error:", err)
            continue
        
        con0_1 = con0_1.replace("\n", "")
        
        data_apm = json.dumps({
            "m2m:cin": {
                "con": con0_1
            }
        })
        
        try:
            r_apm = requests.post(apm_url, headers=headers_apm, data=data_apm)
            r_apm.raise_for_status()
        except requests.exceptions.RequestException as req_err:
            print("APM Request error:", req_err)
        except requests.exceptions.HTTPError as http_err:
            print("APM HTTP error:", http_err)
        except Exception as exc:
            print('APM There was a problem: {}'.format(exc))
    
    time.sleep(55)
