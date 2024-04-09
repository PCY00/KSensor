import requests
import time

def fetch_data(url, timeout=5):
    try:
        response = requests.get(url, timeout=timeout)
        if response.status_code == 200:
            return response.json()
        else:
            print(f"서버 응답 오류: {response.status_code}")
            return None
    except requests.exceptions.ConnectTimeout as e:
        print("연결 시간 초과 오류가 발생했습니다:", e)
        return None
    except requests.exceptions.RequestException as e:
        print("요청 오류가 발생했습니다:", e)
        return None

def process_data(data):
    if 'data' in data and len(data['data']) > 0:
        data_item = data['data'][0]
        date = data_item.get('meastime', '')
        return date
    else:
        print("데이터가 없습니다.")
        return None

url = 'http://211.226.18.66:5002/iot/v1/microdust/measure-last'
previous_date = None

try:
    while True:
        data = fetch_data(url)
        if data:
            date = process_data(data)
            if date and date != previous_date:
                print("새로운 데이터 발견! 날짜:", date)
                previous_date = date
        time.sleep(10)  # 5초마다 반복
except KeyboardInterrupt:
    print("\n프로그램을 종료합니다.")
