import socket
import threading
import serial
import requests
import time
import json
import signal
import sys

# 기존 URL 및 헤더 설정
apm2_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/data'
data_send_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/motor'
rpm_save_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/RPM'  # RPM 저장을 위한 URL
rpm_url = "http://211.226.18.66:5002/iot/v1/microdust/measure-last"

apm2_headers = {
    'Accept': 'application/json',
    'X-M2M-RI': '12345',
    'X-M2M-Origin': 'SKsensor_ubicomp_2',
    'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

# 시리얼 포트 및 보드레이트 설정
ports = ['/dev/ttyACM0', '/dev/ttyACM1', '/dev/ttyACM2', '/dev/ttyACM3', '/dev/ttyACM4']
baudrates = [9600, 9600, 9600, 9600, 9600]

# 시리얼 연결 초기화
serial_connections = []
for port, baudrate in zip(ports, baudrates):
    try:
        ser = serial.Serial(port, baudrate=baudrate, timeout=1)
        serial_connections.append(ser)
        print(f'Connected to {ser.name}.')
    except Exception as err:
        print(f"Serial error on {port}: {err}")

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

def handle_client(conn, addr):
    print('Connected:', addr)
    with conn:
        while running:
            data = conn.recv(1024)
            if not data:
                break
            data = data.decode('utf-8').strip()

            if data == "start":
                for ser in serial_connections:
                    ser.write(data.encode('utf-8'))
                time.sleep(5)

                while running:
                    con0_1 = read_serial_data()
                    if "error" in con0_1:
                        print("Error detected, resending 'start' signal")
                        for ser in serial_connections:
                            ser.write("start".encode('utf-8'))
                        time.sleep(5)
                    else:
                        combined_data = "{},{},{},{}".format(time_s, con0_1, pm_data, pm_data)
                        print(f"{combined_data}")

                        data_list = combined_data.split(',')
                        original_order = [
                            "datetime", "pwm1_1", "pwm1_2", "pwm1_3", "pm1_1", "pm1_2", "pm1_3",
                            "pwm2_1", "pwm2_2", "pwm2_3", "pm2_1", "pm2_2", "pm2_3",
                            "pwm3_1", "pwm3_2", "pwm3_3", "pm3_1", "pm3_2", "pm3_3",
                            "temp", "humi", "o3", "co", "no2", "so2",
                            "wind_d", "wind_s", "npm", "rpm before correction", "rpm after correction"
                        ]

                        desired_order = [
                            "datetime", "rpm before correction", "rpm after correction",
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

                        if len(data_list) != len(original_order):
                            print("Data length mismatch: Retrying.")
                            for ser in serial_connections:
                                ser.write("start".encode('utf-8'))
                            time.sleep(5)
                            continue

                        index_map = {original_order[i]: i for i in range(len(original_order))}
                        reordered_data = [data_list[index_map[col]] for col in desired_order]

                        combined_data = ','.join(reordered_data)

                        print(f"Reordered data: {combined_data}")
                        send_apm_data(combined_data, apm2_url)
                        break

            elif data.startswith('M'):
                # 데이터 'M'으로 시작하는 경우
                for ser in serial_connections:
                    ser.write(data.encode('utf-8'))
                time.sleep(5)

                rpm_data = read_serial_data()
                print(f"Received RPM data: {rpm_data}")
                send_apm_data(rpm_data, rpm_save_url)

            conn.sendall(data.encode('utf-8'))

def server_thread():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((host, port))
        s.listen()
        print(f"Listening on {host}:{port}...")

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
        print(f"HTTP error occurred: {http_err} - {response.status_code}")
        return None
    except Exception as e:
        print(f"Failed to fetch data: {e}")
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
        print("No data received.")
    return time_s, pm_data

def data_thread():
    while running:
        time_s, pm_data = process_data()
        print(time_s)
        print(pm_data)
        
        # Send "start" signal to motor endpoint
        data_apm_start = json.dumps({
            "m2m:cin": {
                "con": "start"
            }
        })
        
        try:
            r_apm_start = requests.post(data_send_url, headers=apm2_headers, data=data_apm_start)
            r_apm_start.raise_for_status()
        except requests.exceptions.RequestException as req_err:
            print("APM request error:", req_err)
        except requests.exceptions.HTTPError as http_err:
            print("APM HTTP error:", http_err)
        except Exception as exc:
            print(f'APM problem occurred: {exc}')
        
        time.sleep(55)

def signal_handler(sig, frame):
    global running
    print('Safely shutting down...')
    running = False
    for ser in serial_connections:
        ser.close()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

# Start server and data threads
server = threading.Thread(target=server_thread)
data_fetch = threading.Thread(target=data_thread)

server.start()
data_fetch.start()

server.join()
data_fetch.join()