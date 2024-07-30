# -*- coding: utf-8 -*-
import requests
import json
import time

def send_to_mobius(data):
    # Mobius 서버 POST 요청 URL
    post_url = 'http://114.71.220.59:7579/Mobius/Ksensor_ubicomp/RPM'
    
    # Mobius 서버에 데이터를 전송하기 위한 헤더
    headers = {
        'Accept': 'application/json',
        'X-M2M-RI': '12345',
        'X-M2M-Origin': 'SqNVGGACjuA',
        'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
    }
    
    # Mobius 서버로 전송할 데이터 구성
    post_data = {
        'm2m:cin': {
            'con': data
        }
    }
    
    # POST 요청 실행
    response = requests.post(post_url, headers=headers, data=json.dumps(post_data))
    
    # 응답 결과 확인
    if response.status_code == 200 or response.status_code == 201:
        print("데이터 Mobius 서버에 전송 성공, {}".format(data))
    else:
        print(f"데이터 Mobius 서버에 전송 실패: {response.status_code}, 메시지: {response.text}")

# 마지막으로 전송한 데이터를 저장할 변수 초기화
last_sent_data = None

try:
    while True:
        # GET 요청 실행
        response = requests.get('http://211.226.18.66:5002/iot/v1/microdust/measure-last')

        # 응답 코드 확인 및 데이터 처리
        if response.status_code == 200:
            # print("데이터 수신 성공!")
            data = response.json()
            if 'data' in data and len(data['data']) > 0:
                data_item = data['data'][0]
                data_str = f"{data_item.get('meastime', '')},{data_item.get('temperature', '')},{data_item.get('humidity', '')},{data_item.get('pm10', '')},{data_item.get('pm2.5', '')},{data_item.get('pm10_a', '')},{data_item.get('pm2.5_a', '')}"
                # print(data_str)
                data = data_str.replace("\n","")
                # 이전에 전송한 데이터와 비교
                if data != last_sent_data:
                    # print("새로운 데이터 발견, Mobius 서버에 전송합니다.")
                    send_to_mobius(data)
                    last_sent_data = data  # 마지막으로 전송한 데이터 업데이트
                # else:
                    # print("데이터 변화 없음, 전송하지 않습니다.")
        else:
            print(f"데이터 수신 실패: {response.status_code}")

except KeyboardInterrupt:
    print("\n프로그램을 종료합니다.")
