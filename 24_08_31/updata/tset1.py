# -*- coding: utf-8 -*-
# pip install schedule
import socket
from datetime import datetime
import serial
import requests
import time
import json
import signal
import sys
import threading
import schedule

# 기존 URL 및 헤더 설정
apm2_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/data'
data_send_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/motor'
rpm_url = "http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/RPMData"
FanSpeed_url = "http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/FanSpeed"

sejong_url = "http://114.71.220.59:2021/Mobius/SMLplatform/Sensor/train"

apm2_headers = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': 'SKsensor_ubicomp_2',
    'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

sejong_headers = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': 'SKsensor',
    'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

# 시리얼 포트 및 보드레이트 설정
serial_port_0 = '/dev/ttyM1'
serial_port_1 = '/dev/ttyM2'
serial_port_2 = '/dev/ttyM3'
serial_port_3 = '/dev/ttySensor'
serial_port_4 = '/dev/ttyNPM'

baudrate = 9600

# 소켓 설정
port = 65432

#전역 변수 설정
is_restoring_fan_state = False  # True: 복원 중, False: 복원 중이 아님
retry = True                    # 키보드 인터럽트 발생 시 서버 종료를 위한 변수

# 데이터 형식 :  ["RPMdatatime, APMdatetime", "rpm before correction", "rpm after correction",
#                            "pwm1_1", "pwm1_2", "pwm1_3",
#                            "pwm2_1", "pwm2_2", "pwm2_3",
#                            "pwm3_1", "pwm3_2", "pwm3_3",
#                            "npm",
#                            "pm1_1", "pm1_2", "pm1_3",
#                            "pm2_1", "pm2_2", "pm2_3",
#                            "pm3_1", "pm3_2", "pm3_3",
#                            "temp", "humi", "o3", "co",
#                            "no2", "so2", "wind_d", "wind_s"]
save_data = []

# 임계영역 보호를 위한 Lock 생성 ( 데이터 수집과 제어 명령어 전송과의 충돌 방지 )
lock = threading.Lock()


# 시리얼 연결 초기화
try:
    ser0 = serial.Serial(serial_port_0, baudrate=baudrate, timeout=1)
    ser1 = serial.Serial(serial_port_1, baudrate=baudrate, timeout=1)
    ser2 = serial.Serial(serial_port_2, baudrate=baudrate, timeout=1)
    ser3 = serial.Serial(serial_port_3, baudrate=baudrate, timeout=1)
    ser4 = serial.Serial(serial_port_4, baudrate=baudrate, timeout=1)
    print(f'Connected to {ser0.name},{ser1.name},{ser2.name},{ser3.name},{ser4.name}')
except Exception as err:
    print(f"Serial error: {err}")
    sys.exit(1)

# 아두이노 버퍼 비워주는 함수
def clear_serial_buffer():
    ser0.reset_input_buffer()
    ser1.reset_input_buffer()
    ser2.reset_input_buffer()
    ser3.reset_input_buffer()
    ser4.reset_input_buffer()
    print("Serial buffers cleared")
    
# 아두이노 각각의 버퍼 비워주는 함수    
def each_serial_buffer(each_serial_port):
    each_serial_port.reset_input_buffer()
    print("Serial buffers cleared")
    

# 아두이노 처음 시작했을 때 초기화함수
def init_serial():
    ser0.write(b'0')
    ser1.write(b'0')
    ser2.write(b'0')
    ser3.write(b'0')
    ser4.write(b'0')
    clear_serial_buffer()
    print("loading....")
    time.sleep(5)

# 아두이노에서 데이터 수신 오류시 재시도 함수    
def try_send_data(try_serial_port):
    try:
        each_serial_buffer(try_serial_port)
        try_serial_port.write(b'start')
        time.sleep(5)
        data = try_serial_port.readline().decode('utf-8').strip()
        if data == "":
            return "null"
        return data
    except Exception as try_err:
        print(f"Serial send error: {try_err}")
        return "null"
    
# 아두이노에서 피드백 수신 오류시 재시도 함수
def try_send_feedback(try_serial_port, data):
    try:
        each_serial_buffer(try_serial_port)
        try_serial_port.write(data.encode('utf-8'))
        time.sleep(10)
        data = try_serial_port.readline().decode('utf-8').strip()
        if data == "":
            return "check the fan go to 7 floor"
        return data
    except Exception as try_err:
        print(f"Serial send error: {try_err}")
        return "check the fan go to 7 floor"
    
# 시리얼 데이터 수신 함수
def read_serial_data():
    try:
        # 각 시리얼 포트에 대해 데이터가 수신되었는지 확인
        if ser0.in_waiting > 0:
            data0 = ser0.readline().decode('utf-8').strip()
        else:
            # 만약 데이터가 없다면 재시도
            data0 = try_send_data(ser0)

        if ser1.in_waiting > 0:
            data1 = ser1.readline().decode('utf-8').strip()
        else:
            data1 = try_send_data(ser1)

        if ser2.in_waiting > 0:
            data2 = ser2.readline().decode('utf-8').strip()
        else:
            data2 = try_send_data(ser2)

        if ser3.in_waiting > 0:
            data3 = ser3.readline().decode('utf-8').strip()
        else:
            data3 = try_send_data(ser3)

        if ser4.in_waiting > 0:
            data4 = ser4.readline().decode('utf-8').strip()
        else:
            data4 = try_send_data(ser4)

        # 수신된 데이터를 포맷팅하여 반환
        data = "{}{}{}{},{}".format(data0, data1, data2, data3, data4)
        if data0 == " , " or data1 == " , " or data2 == " , " or data3 == " , " or data4 == " , ":
            return "null"
        elif data == "null":
            return "null"
        return data
    except Exception as read_err:
        print(f"Serial read error: {read_err}")
        return "error"

# APM 데이터 전송 함수
def send_apm_data(data, url):
    data_apm = json.dumps({
        "m2m:cin": {
            "con": data
        }
    })
    
    try:
        r_apm2 = requests.post(url, headers=apm2_headers, data=data_apm)
        r_apm2.raise_for_status()
    except requests.exceptions.RequestException as req_err:
        print("APM request error:", req_err)
    except requests.exceptions.HTTPError as http_err:
        print("APM HTTP error:", http_err)
    except Exception as exc:
        print(f'APM problem occurred: {exc}')
        
# 750, 1200, 1650, 2100, 2550, 3060, 3420  
def send_sejong_data(data, url):
    where_list = data.split(',')
    
    data_apm = json.dumps({
        "m2m:cin": {
            "con": data
        }
    })
    
    
    if where_list[3] == '900':
        new_url = url + "/param1"
    elif where_list[3] == '1200':
        new_url = url + "/param2"
    elif where_list[3] == '1650':
        new_url = url + "/param3"
    elif where_list[3] == '2100':
        new_url = url + "/param4"
    elif where_list[3] == '2550':
        new_url = url + "/param5"        
    elif where_list[3] == '3060':
        new_url = url + "/param6"
    elif where_list[3] == '3420':
        new_url = url + "/param7"
    else:
        print("fanSpeed selection error")

    try:
        r_apm2 = requests.post(new_url, headers=sejong_headers, data=data_apm)
        r_apm2.raise_for_status()
    except requests.exceptions.RequestException as req_err:
        print("Sejong request error:", req_err)
    except requests.exceptions.HTTPError as http_err:
        print("Sejong HTTP error:", http_err)
    except Exception as exc:
        print(f'Sejong problem occurred: {exc}')
        
def CommandFanControl(data):
    
    lock.acquire()
    clear_serial_buffer()
    if data[1] == '1':                  # 모듈 번호 1
        ser0.write(data.encode('utf-8'))
        time.sleep(10)                  # fanSpeed가 명령한 speed에 도달하고 피드백을 주기위한 시간 지연
        # 피드백이 없는 경우 다시 명령어를 보내 피드백을 받기 위해 시도, 10초 후에도 없으면 "check the fan go to 7 floor" 반환
        FanSpeed_data = ser0.readline().decode('utf-8').strip() if ser0.in_waiting > 0 else try_send_feedback(ser0, data)
    elif data[1] == '2':                # 모듈 번호 2
        ser1.write(data.encode('utf-8'))
        time.sleep(10)                  # fanSpeed가 명령한 speed에 도달하고 피드백을 주기위한 시간 지연
        # 피드백이 없는 경우 다시 명령어를 보내 피드백을 받기 위해 시도, 10초 후에도 없으면 "check the fan go to 7 floor" 반환
        FanSpeed_data = ser1.readline().decode('utf-8').strip() if ser1.in_waiting > 0 else try_send_feedback(ser1, data)
    elif data[1] == '3':                # 모듈 번호 3
        ser2.write(data.encode('utf-8'))
        time.sleep(10)                  # fanSpeed가 명령한 speed에 도달하고 피드백을 주기위한 시간 지연
        # 피드백이 없는 경우 다시 명령어를 보내 피드백을 받기 위해 시도, 10초 후에도 없으면 "check the fan go to 7 floor" 반환
        FanSpeed_data = ser2.readline().decode('utf-8').strip() if ser2.in_waiting > 0 else try_send_feedback(ser2, data)

    print(f"Received Fanspeed data: {FanSpeed_data}")
    send_apm_data(FanSpeed_data, FanSpeed_url)

    print(f"lock released")
    lock.release()

                        
    
    
def OneMinute(url):
    global RPM_pm_data_b, RPM_pm_data_a, RPM_time
    
    lock.acquire()
    #APM2 시간 수집
    APM2_time = datetime.now().strftime('%Y%m%d%H%M%S')
    
    #RPM 데이터 수집
    rpm_data_get = requests.get(url, headers = apm2_headers)
        
    try:
        rpm_data_get.raise_for_status()
        rpm_data_get_json = rpm_data_get.json()
        rpm_data_original = rpm_data_get_json.get("m2m:cin", {}).get("con", None)
    except Exception as exc:
        print(f"There was a problem: {exc}")
        rpm_data_original = None

    # RPM 데이터 처리
    if rpm_data_original:
        rpm_data_original_list = rpm_data_original.split(',')
        RPM_time = rpm_data_original_list[0] if len(rpm_data_original_list) > 0 else 'null'
        RPM_pm_data_b = rpm_data_original_list[5] if len(rpm_data_original_list) > 5 else 'null'
        RPM_pm_data_a = rpm_data_original_list[6] if len(rpm_data_original_list) > 6 else 'null'
    else:
        RPM_time = 'null'
        RPM_pm_data_b = 'null'
        RPM_pm_data_a = 'null'
        
        #APM2 데이터 수집
        clear_serial_buffer()
        print(f"sending start")
        ser0.write(b'start')
        ser1.write(b'start')
        ser2.write(b'start')
        ser3.write(b'start')
        ser4.write(b'start')
        time.sleep(5)
        
        APM2_Data = read_serial_data()
        combined_data = "{},{},{},{}".format(APM2_time, RPM_time, APM2_Data, RPM_pm_data_b, RPM_pm_data_a)
        print(f"{combined_data}")

        data_list = combined_data.split(',')
        
        #데이터 재정렬
        original_order = [
            "APM_datetime", "RPM_datetime", "pwm1_1", "pwm1_2", "pwm1_3", "pm1_1", "pm1_2", "pm1_3",
            "pwm2_1", "pwm2_2", "pwm2_3", "pm2_1", "pm2_2", "pm2_3",
            "pwm3_1", "pwm3_2", "pwm3_3", "pm3_1", "pm3_2", "pm3_3",
            "temp", "humi", "o3", "co", "no2", "so2",
            "wind_d", "wind_s", "npm", "rpm before correction", "rpm after correction"
        ]
        desired_order = [
            "APM_datetime", "RPM_datetime", "rpm before correction", "rpm after correction",
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
        
        index_map = {original_order[i]: i for i in range(len(original_order))}
        reordered_data = [data_list[index_map[col]] for col in desired_order]
        combined_data = ','.join(reordered_data)
        print(f"Reordered data: {combined_data}")
        
        #apm2 server 데이터 전송
        send_apm_data(combined_data, apm2_url)
        
        #sejong server 데이터 전송
        send_sejong_data(combined_data, sejong_url)
    
    lock.release()

# TCP통신 함수
def server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(('localhost', port))
        s.listen()
        print(f"Listening on port {port}...")

        while retry:
            try:       
                conn, addr = s.accept()
                with conn:
                    print(f'Connected to {addr}')
                    while retry:
                        # 데이터 크기
                        data = conn.recv(1024)

                        # TCP로 데이터가 오지 않으면 서버 종료
                        if not data:
                            break
                        data = data.decode('utf-8').strip()
                        if data.startswith('M'):
                            CommandFanControl(data)
                            
                        #60초마다 데이터를 전송하기 위한 스케줄러 (람다를 써야 함수에 인자를 넣을 수 있음, 함수의 결과값을 넣을 수 없음)
                        schedule.every(60).seconds.do(lambda: OneMinute(rpm_url + '/la'))
            except Exception as server_err:
                print(f"Server error: {server_err}")
                retry = False
                
def signal_handler(sig, frame):
    global running
    print('Safely shutting down...')
    running = False
    ser0.close()
    ser1.close()
    ser2.close()
    ser3.close()
    ser4.close()
    
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)


# 코드 실행 시작 부분
init_serial()
clear_serial_buffer()

try:
    server()
except KeyboardInterrupt:
    print("KeyboardInterrupt")
    ser0.close()
    ser1.close()
    ser2.close()
    ser3.close()
    ser4.close()
    sys.exit(0)
