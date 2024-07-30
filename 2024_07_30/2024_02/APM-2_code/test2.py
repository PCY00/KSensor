# -*- coding: utf-8 -*-
import socket
import threading
import serial
import requests
import time
import json
import signal
import sys

# URL과 헤더 설정
apm2_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/data'
data_send_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/motor'
rpm_url = "http://211.226.18.66:5002/iot/v1/microdust/measure-last"

apm2_headers = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': 'SKsensor_ubicomp_2',
    'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

# 시리얼 포트와 보드레이트 설정
ports = ['/dev/ttyACM0', '/dev/ttyACM1', '/dev/ttyACM2', '/dev/ttyACM3', '/dev/ttyACM4']
baudrates = [9600, 9600, 9600, 9600, 9600]

# 시리얼 연결 초기화
serial_connections = []
for port, baudrate in zip(ports, baudrates):
    try:
        ser = serial.Serial(port, baudrate=baudrate, timeout=1)
        serial_connections.append(ser)
        print(f'{ser.name}에 연결되었습니다.')
    except Exception as err:
        print(f"{port}에서 시리얼 오류 발생: {err}")

# 서버 설정
host = 'localhost'
port = 65432

# 전역 변수 초기화
time_s, pm_data = "", ""
last_sent_data, pm_data_save = "", ""
running = True

def read_serial_data():
    try:
        data = [ser.readline().decode('utf-8').strip() if ser.readable() else "" for ser in serial_connections]
        con0_1 = "{}{}{}{},{}".format(*data[:4], data[4])
        if con0_1.strip() == " , ":
            return "error"
        return con0_1.replace("\n", "")
    except Exception as read_err:
        print(f"시리얼 읽기 오류: {read_err}")
        return "error"

def send_apm_data(data):
    data_apm = json.dumps({
        "m2m:cin": {
            "con": data
        }
    })

    try:
        r_apm2 = requests.post(apm2_url, headers=apm2_headers, data=data_apm)
        r_apm2.raise_for_status()
    except requests.exceptions.RequestException as req_err:
        print("APM 요청 오류:", req_err)
    except requests.exceptions.HTTPError as http_err:
        print("APM HTTP 오류:", http_err)
    except Exception as exc:
        print(f'APM 문제가 발생했습니다: {exc}')

def handle_client(conn, addr):
    print('연결됨:', addr)
    with conn:
        while running:
            data = conn.recv(1024)
            if not data:
                break
            data = data.decode('utf-8').strip()

            for ser in serial_connections:
                ser.write(data.encode('utf-8'))
            time.sleep(5)

            if data == "start":
                while running:
                    con0_1 = read_serial_data()
                    if "error" in con0_1:
                        print("오류 감지됨, 'start' 신호 재전송")
                        for ser in serial_connections:
                            ser.write("start".encode('utf-8'))
                        time.sleep(5)
                    else:
                        #print(f"받은 데이터: {con0_1}")
                        # combined_data를 올바르게 구성하기 위해, time_s, con0_1, pm_data를 출력하여 확인

                        combined_data = "{},{},{},{}".format(time_s, con0_1, pm_data,pm_data)
                        
                        print(f"{combined_data}")
                        
                        data_list = combined_data.split(',')
                        original_order = [
                           "datetime", "pwm1_1", "pwm1_2", "pwm1_3", "pm1_1", "pm1_2", "pm1_3",
                           "pwm2_1", "pwm2_2", "pwm2_3", "pm2_1", "pm2_2", "pm2_3",
                           "pwm3_1", "pwm3_2", "pwm3_3", "pm3_1", "pm3_2", "pm3_3",
                           "temp", "humi", "o3", "co", "no2", "so2",
                           "wind_d", "wind_s", "npm", "rpm보정전", "rpm 보정후"
                        ]
                        
                        desired_order = [
                            "datetime", "rpm보정전", "rpm 보정후",
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
                        
                        # data_list의 길이가 original_order와 맞지 않을 경우 빈 문자열로 채우기
                        if len(data_list) != len(original_order):
                            print("데이터 길이 불일치: 다시 시도합니다.")
                            for ser in serial_connections:
                                ser.write("start".encode('utf-8'))
                            time.sleep(5)
                            continue

                        index_map = {original_order[i]: i for i in range(len(original_order))}
                        reordered_data = [data_list[index_map[col]] for col in desired_order]
                        
                        combined_data = ','.join(reordered_data)
                        
                        print(f"재구성된 데이터: {combined_data}")
                        send_apm_data(combined_data)
                        break

            conn.sendall(data.encode('utf-8'))

def server_thread():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((host, port))
        s.listen()
        print(f"{host}:{port}에서 듣고 있습니다...")

        while running:
            conn, addr = s.accept()
            client_thread = threading.Thread(target=handle_client, args=(conn, addr))
            client_thread.start()

def fetch_data():
    try:
        response = requests.get(rpm_url)
        response.raise_for_status()
        return response.json()
    except requests.HTTPError as http_err:
        print(f"HTTP 오류 발생: {http_err} - {response.status_code}")
        return None
    except Exception as e:
        print(f"데이터 가져오기 실패: {e}")
        return None

def process_data():
    global last_sent_data, pm_data_save, time_s, pm_data

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
                    pm_data = pm2_5_a

                    return time_s, pm_data
    else:
        print("데이터를 받지 못했습니다.")
    return time_s, pm_data

def data_thread():
    while running:
        time_s, pm_data = process_data()
        print(time_s)
        print(pm_data)
        
        # 모터 엔드포인트에 "start" 신호 전송
        data_apm_start = json.dumps({
            "m2m:cin": {
                "con": "start"
            }
        })
        
        try:
            r_apm_start = requests.post(data_send_url, headers=apm2_headers, data=data_apm_start)
            r_apm_start.raise_for_status()
        except requests.exceptions.RequestException as req_err:
            print("APM 요청 오류:", req_err)
        except requests.exceptions.HTTPError as http_err:
            print("APM HTTP 오류:", http_err)
        except Exception as exc:
            print(f'APM 문제가 발생했습니다: {exc}')
        
        time.sleep(60)

def signal_handler(sig, frame):
    global running
    print('안전하게 종료 중...')
    running = False
    for ser in serial_connections:
        ser.close()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

# 서버와 데이터 스레드 시작
server = threading.Thread(target=server_thread)
data_fetch = threading.Thread(target=data_thread)

server.start()
data_fetch.start()

server.join()
data_fetch.join()

