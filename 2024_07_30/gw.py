import socket
import serial
import requests
import time
import json
import signal
import sys
from threading import Lock

# 기존 URL 및 헤더 설정
apm2_url = 'http://114.71.220.59:2021/Mobius/gw/data'
data_send_url = 'http://114.71.220.59:2021/Mobius/gw/motor'
rpm_url = "http://114.71.220.59:2021/Mobius/gw/RPMData"
FanSpeed_url = "http://114.71.220.59:2021/Mobius/gw/FanSpeed"

apm2_headers = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': 'Sgw',
    'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

# 시리얼 포트 및 보드레이트 설정
serial_port_0 = '/dev/ttyACM0'
baudrate = 9600

# 시리얼 연결 초기화
try:
    ser0 = serial.Serial(serial_port_0, baudrate=baudrate, timeout=1)
    print(f'Connected to {ser0.name}.')
except Exception as err:
    print(f"Serial error on {serial_port_0}: {err}")
    sys.exit(1)
    
def clear_serial_buffer():
    ser0.reset_input_buffer()
    print("Serial buffers cleared")

first = 0
# 초기화 시 메시지 전송
if first == 0:
    ser0.write(b'0')
    first = 1
    clear_serial_buffer()
    print("loading....")
    time.sleep(5)

# 서버 설정
port = 12345

# 전역 변수 초기화
time_s, pm_data, pm_data_a = "", "", ""
running = True

# 임계영역 보호를 위한 Lock 생성
lock = Lock()

def read_serial_data():
    try:
        data = ser0.readline().decode('utf-8').strip()
        if data == " , ":
            return "error"
        return data
    except Exception as read_err:
        print(f"Serial read error: {read_err}")
        return "error"

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

def process_data(data):
    global time_s, pm_data, pm_data_a

    with lock:  # 임계영역 시작
        if data.startswith('M'):
            clear_serial_buffer()
            ser0.write(data.encode('utf-8'))
            time.sleep(10)

            rpm_data = read_serial_data()
            print(f"Received RPM data: {rpm_data}")
            send_apm_data(rpm_data, FanSpeed_url)

        elif data.startswith('2'):
            rpm_data_list = data.split(',')
            time_s, pm_data, pm_data_a = rpm_data_list[0], rpm_data_list[5], rpm_data_list[6]
            
            print(f"Data starts with '2': {time_s},{pm_data},{pm_data_a}\n")
            clear_serial_buffer()
            print(f"snding start")
            ser0.write(b'start')
            time.sleep(5)
            
            con0_1 = read_serial_data()
            if "error" in con0_1:
                clear_serial_buffer()
                print("Error detected, resending 'start' signal")
                ser0.write("start".encode('utf-8'))
                time.sleep(5)
            else:
                if time_s and pm_data and pm_data_a:
                    combined_data = "{},{},{},{}".format(time_s, con0_1, pm_data, pm_data_a)
                    print(f"{combined_data}")

                    data_list = combined_data.split(',')
                    original_order = [
                        "datetime", "pwm1_1", "pwm1_2", "pwm1_3", 
                        "pm1_1", "pm1_2", "pm1_3", 
                        "rpm before correction", "rpm after correction"
                    ]

                    desired_order = [
                        "datetime", "rpm before correction", "rpm after correction",
                        "pwm1_1", "pwm1_2", "pwm1_3",
                        "pm1_1", "pm1_2", "pm1_3",
                    ]

                    if len(data_list) != len(original_order):
                        clear_serial_buffer()
                        print("Data length mismatch: Retrying.")
                        ser0.write("start".encode('utf-8'))
                        time.sleep(5)

                    index_map = {original_order[i]: i for i in range(len(original_order))}
                    reordered_data = [data_list[index_map[col]] for col in desired_order]

                    combined_data = ','.join(reordered_data)

                    print(f"Reordered data: {combined_data}")
                    send_apm_data(combined_data, apm2_url)
                else:
                    print("Global variables are not yet initialized.")
        # 임계영역 끝

def server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(('localhost', port))
        s.listen()
        print(f"Listening on port {port}...")

        while running:
            conn, addr = s.accept()
            with conn:
                print(f'Connected to {addr}')
                while running:
                    data = conn.recv(1024)
                    if not data:
                        break
                    data = data.decode('utf-8').strip()
                    process_data(data)

def signal_handler(sig, frame):
    global running
    print('Safely shutting down...')
    running = False
    ser0.close()
    sys.exit(0)


signal.signal(signal.SIGINT, signal_handler)

# 서버 실행
try:
    server()
except Exception as e:
    print(f"Server error: {e}")
finally:
    ser0.close()
    print("Serial connection closed.")
