import requests
import serial
import time

# 시리얼 포트와 속도 설정
serial_port = '/dev/ttyUSB0'  # 아두이노와 연결된 포트
baud_rate = 9600

# 시리얼 포트 초기화
ser = serial.Serial(serial_port, baud_rate)

previous_timestamp = None

def get_data_from_mobius():
    try:
        url = 'http://203.253.128.177:7579/Mobius/20191546/ss/su/la'
        headers = {
            'Accept': 'application/json',
            'X-M2M-RI': '12345',
            'X-M2M-Origin': '20191546',
            'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
        }

        r = requests.get(url, headers=headers)
        r.raise_for_status()
        jr = r.json()

        return jr["m2m:cin"]["con"], jr["m2m:cin"]["ct"]
        
    except Exception as exc:
        print("There was a problem:", exc)
        return None, None

# 주기적으로 데이터 확인
while True:
    received_value, received_timestamp = get_data_from_mobius()
    if received_value is not None and received_timestamp is not None:
        if received_timestamp != previous_timestamp:
            ser.write(received_value.encode())
            print("Received new value:", received_value)
            previous_timestamp = received_timestamp
        else:
            print("No new data.")
    time.sleep(10)  # 5초마다 데이터 확인
    
    

