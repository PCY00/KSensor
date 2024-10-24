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
from collections import OrderedDict
import schedule

# 기존 URL 및 헤더 설정
apm2_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/data'
data_send_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/motor'
rpm_url = "http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/RPMData"
FanSpeed_url = "http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/FanSpeed"

sejong_url = "http://114.71.220.59:2021/Mobius/SMLplatform/Sensor/train"
sejong_url_module = "http://114.71.220.59:2021/Mobius/SMLplatform/Sensor"

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
    
    new_data_list = []
    for item in where_list:
        try:
            new_data_list.append(float(item))
        except ValueError:
            new_data_list.append(item)
            
    con0_1_sejong_json = OrderedDict([
        ("label", float(where_list[3])),
        ("data", new_data_list)
    ])
    con0_1_sejong_str = json.dumps(con0_1_sejong_json)
    
    
    if where_list[4] == '900':
        new_url = url + "/param1"
    elif where_list[4] == '1200':
        new_url = url + "/param2"
    elif where_list[4] == '1650':
        new_url = url + "/param3"
    elif where_list[4] == '2100':
        new_url = url + "/param4"
    elif where_list[4] == '2550':
        new_url = url + "/param5"        
    elif where_list[4] == '3060':
        new_url = url + "/param6"
    elif where_list[4] == '3420':
        new_url = url + "/param7"
    else:
        print("fanSpeed selection error")
        
    data_sejong = json.dumps({
        "m2m:cin": {
            "con": con0_1_sejong_str
        }
    })

    try:
        r_apm2 = requests.post(new_url, headers=sejong_headers, data=data_sejong)
        r_apm2.raise_for_status()
    except requests.exceptions.RequestException as req_err:
        print("Sejong request error:", req_err)
    except requests.exceptions.HTTPError as http_err:
        print("Sejong HTTP error:", http_err)
    except Exception as exc:
        print(f'Sejong problem occurred: {exc}')

# 모듈 데이터 처리 함수
def process_module_data(module):
    module_arr = []
    for item in module:
        try:
            module_arr.append(float(item))
        except ValueError:
            module_arr.append(item)
    return module_arr

# 모듈 JSON 데이터 생성 함수
def create_sejong_json(label, module_data):
    sejong_json = OrderedDict([
        ("label", float(label)),
        ("data", module_data)
    ])
    return json.dumps(sejong_json)

# 세종 서버로 데이터 전송 함수
def send_sejong_request(module_data, module_num, url):
    data_sejong = json.dumps({
        "m2m:cin": {
            "con": module_data
        }
    })
    response = requests.post(f"{url}/module_{module_num}", headers=sejong_headers, data=data_sejong)
    return response

# 세종 서버로 데이터 전송 함수 (모듈 1, 2, 3)
def send_sejong_data_two(data, url):
    where_list = data.split(',')
    
    # 각 모듈의 데이터를 추출
    module_1 = where_list[:6] + where_list[13:16] + where_list[22:]
    module_2 = where_list[:3] + where_list[6:9] + where_list[16:19] + where_list[22:]
    module_3 = where_list[:3] + where_list[9:12] + where_list[19:22] + where_list[22:]

    # 데이터 처리
    module_1_arr = process_module_data(module_1)
    module_2_arr = process_module_data(module_2)
    module_3_arr = process_module_data(module_3)

    # JSON 데이터 생성
    con0_1_sejong_str = create_sejong_json(where_list[2], module_1_arr)
    con0_2_sejong_str = create_sejong_json(where_list[2], module_2_arr)
    con0_3_sejong_str = create_sejong_json(where_list[2], module_3_arr)

    # 서버로 데이터 전송
    send_sejong_request(con0_1_sejong_str, 1, url)
    send_sejong_request(con0_2_sejong_str, 2, url)
    send_sejong_request(con0_3_sejong_str, 3, url)

        
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

    print("Acquiring lock")
    lock.acquire()
    
    try:
        # APM2 시간 수집
        APM2_time = datetime.now().strftime('%Y%m%d%H%M%S')
        
        # RPM 데이터 수집
        print("Fetching RPM data")
        rpm_data_get = requests.get(url, headers=apm2_headers)
        rpm_data_get.raise_for_status()
        rpm_data_get_json = rpm_data_get.json()
        rpm_data_original = rpm_data_get_json.get("m2m:cin", {}).get("con", None)
        print(f"RPM data fetched: {rpm_data_original}")

        # RPM 데이터 처리
        if rpm_data_original:
            rpm_data_original_list = rpm_data_original.split(',')
            RPM_time = rpm_data_original_list[0] if len(rpm_data_original_list) > 0 else 'null'
            RPM_pm_data_b = rpm_data_original_list[5] if len(rpm_data_original_list) > 5 else 'null'
            RPM_pm_data_a = rpm_data_original_list[6] if len(rpm_data_original_list) > 6 else 'null'
        else:
            RPM_time = RPM_pm_data_b = RPM_pm_data_a = 'null'

    # APM2 데이터 수집 및 전송
        clear_serial_buffer()
        print("Sending start command to serial ports")
        for ser in [ser0, ser1, ser2, ser3, ser4]:
            ser.write(b'start')
        time.sleep(5)
        
        APM2_Data = read_serial_data()
        combined_data = f"{APM2_time},{RPM_time},{APM2_Data},{RPM_pm_data_b},{RPM_pm_data_a}"
        print(f"Combined data: {combined_data}")
        
        # 데이터 재정렬
        original_order = [
            "APM_datetime", "RPM_datetime", "pwm1_1", "pwm1_2", "pwm1_3", "pm1_1", "pm1_2", "pm1_3",
            "pwm2_1", "pwm2_2", "pwm2_3", "pm2_1", "pm2_2", "pm2_3",
            "pwm3_1", "pwm3_2", "pwm3_3", "pm3_1", "pm3_2", "pm3_3",
            "temp", "humi", "o3", "co", "no2", "so2",
            "wind_d", "wind_s", "npm", "rpm before correction", "rpm after correction"
        ]
        desired_order = [
            "APM_datetime", "RPM_datetime", "rpm before correction", "rpm after correction",
            "pwm1_1", "pwm1_2", "pwm1_3", "pwm2_1", "pwm2_2", "pwm2_3",
            "pwm3_1", "pwm3_2", "pwm3_3", "npm", "pm1_1", "pm1_2", "pm1_3",
            "pm2_1", "pm2_2", "pm2_3", "pm3_1", "pm3_2", "pm3_3",
            "temp", "humi", "o3", "co", "no2", "so2", "wind_d", "wind_s"
        ]

        index_map = {original_order[i]: i for i in range(len(original_order))}
        data_list = combined_data.split(',')
        reordered_data = [data_list[index_map[col]] for col in desired_order]
        combined_data = ','.join(reordered_data)
        print(f"Reordered data: {combined_data}")
        
        # APM2, Sejong 서버 및 모듈로 데이터 전송
        send_apm_data(combined_data, apm2_url)
        send_sejong_data(combined_data, sejong_url)
        send_sejong_data_two(combined_data, sejong_url_module)
    except Exception as exc:
        print(f"Error occurred: {exc}")
    finally:
        print("Releasing lock")
        lock.release()



# TCP통신 함수
def handle_connection(conn, addr):
    print(f'Connected to {addr}')
    while True:
        data = conn.recv(1024)
        if not data:
            break
        data = data.decode('utf-8').strip()
        if data.startswith('M'):
            CommandFanControl(data)
    conn.close()

def schedule_tasks():
    while True:
        time.sleep(60)
        print("time's up")
        OneMinute(rpm_url + '/la')
        

def server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(('localhost', port))
        s.listen()
        print(f"Listening on port {port}...")

        # 스케줄 작업을 별도의 스레드에서 실행
        scheduler_thread = threading.Thread(target=schedule_tasks, daemon=True)
        scheduler_thread.start()

        while True:
            conn, addr = s.accept()
            client_thread = threading.Thread(target=handle_connection, args=(conn, addr))
            client_thread.start()
                
def signal_handler(sig, frame):
    global running
    print('Safely shutting down...')
    running = False
    ser0.close()
    ser1.close()
    ser2.close()
    ser3.close()
    ser4.close()
    #who are you dont touch
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
