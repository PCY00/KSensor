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
# Sensor
port3 = '/dev/ttyACM3'
# NPM
port4 = '/dev/ttyACM4'

apm2_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/data'
rpm_save_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/RPM'
data_send_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/motor'
rpm_url = "http://211.226.18.66:5002/iot/v1/microdust/measure-last"

brate0 = 9600
brate1 = 9600
brate2 = 9600
brate3 = 9600
brate4 = 9600

apm2_headers = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': 'SKsensor_ubicomp_2',
    'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

try:
    ser0 = serial.Serial(port0, baudrate=brate0, timeout=None)
    ser1 = serial.Serial(port1, baudrate=brate1, timeout=None)
    ser2 = serial.Serial(port2, baudrate=brate2, timeout=None)
    ser3 = serial.Serial(port3, baudrate=brate3, timeout=None)
    ser4 = serial.Serial(port4, baudrate=brate4, timeout=None)
    print('%s and %s and %s and %s and %s' % (ser0.name, ser1.name, ser2.name, ser3.name, ser4.name))
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
    global last_sent_data, pm_data_save, ser0, ser1, ser2, ser3, ser4, time_s, pm_data

    contect0 = contect1 = contect2 = contect3 = contect4 = ""
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

                    send_1 = "start_m1"
                    send_2 = "start_m2"
                    send_3 = "start_m3"
                    send_4 = "start_sensor"
                    send_5 = "start_npm"

                    con_1 = "{\n    \"m2m:cin\": {\n        \"con\": \"" + send_1 + "\"\n    }\n}"
                    con_2 = "{\n    \"m2m:cin\": {\n        \"con\": \"" + send_2 + "\"\n    }\n}"
                    con_3 = "{\n    \"m2m:cin\": {\n        \"con\": \"" + send_3 + "\"\n    }\n}"
                    con_4 = "{\n    \"m2m:cin\": {\n        \"con\": \"" + send_4 + "\"\n    }\n}"
                    con_5 = "{\n    \"m2m:cin\": {\n        \"con\": \"" + send_5 + "\"\n    }\n}"

                    s1_apm2 = requests.post(data_send_url, headers=apm2_headers, data=con_1)
                    s2_apm2 = requests.post(data_send_url, headers=apm2_headers, data=con_2)
                    s3_apm2 = requests.post(data_send_url, headers=apm2_headers, data=con_3)
                    s4_apm2 = requests.post(data_send_url, headers=apm2_headers, data=con_4)
                    s5_apm2 = requests.post(data_send_url, headers=apm2_headers, data=con_5)

                    return time_s, pm_data
    else:
        print("No data received")
    return time_s, pm_data

while True:
    time_s, pm_data = process_data()
    print(time_s)
    print(pm_data)

    if ser0.in_waiting and ser1.in_waiting and ser2.in_waiting and ser3.in_waiting and ser4.in_waiting:
        try:
            contect0 = ser0.readline().decode('utf-8').strip()
            contect1 = ser1.readline().decode('utf-8').strip()
            contect2 = ser2.readline().decode('utf-8').strip()
            contect3 = ser3.readline().decode('utf-8').strip()
            contect4 = ser4.readline().decode('utf-8').strip()
            
            print("start")
            print(contect0)
            print(contect1)
            print(contect2)
            print(contect3)
            print(contect4)
            con0_1 = "{},{},{},{},{},{},{},{}".format(time_s, contect0, contect1, contect2, contect3, contect4, pm_data, pm_data)

            # 데이터 정리
            data_list = con0_1.split(',')

            # 원래 데이터 순서 (0부터 시작하는 인덱스)
            original_order = [
                "datetime", "pwm1_1", "pwm1_2", "pwm1_3", "pm1_1", "pm1_2", "pm1_3",
                "pwm2_1", "pwm2_2", "pwm2_3", "pm2_1", "pm2_2", "pm2_3",
                "pwm3_1", "pwm3_2", "pwm3_3", "pm3_1", "pm3_2", "pm3_3",
                "temp", "humi", "o3", "co", "no2", "so2",
                "wind_d", "wind_s", "npm", "rpm보정 전", "rpm 보정후"
            ]

            # 원하는 순서 (0부터 시작하는 인덱스)
            desired_order = [
                "datetime", "rpm보정 전", "rpm 보정후",
                "pwm1_1", "pwm1_2", "pwm1_3",
                "pwm2_1", "pwm2_2", "pwm2_3",
                "pwm3_1", "pwm3_2", "pwm3_3",
                "npm",
                "pm1_1", "pm1_2", "pm1_3",
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
            r_apm2 = requests.post(apm2_url, headers=apm2_headers, data=data_apm)
            r_apm2.raise_for_status()
        except requests.exceptions.RequestException as req_err:
            print("APM Request error:", req_err)
        except requests.exceptions.HTTPError as http_err:
            print("APM HTTP error:", http_err)
        except Exception as exc:
            print('APM There was a problem: {}'.format(exc))

    time.sleep(60)

