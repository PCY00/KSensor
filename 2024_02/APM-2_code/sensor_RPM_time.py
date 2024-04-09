import serial
import requests
import time

port0 = '/dev/ttyACM0'
port1 = '/dev/ttyACM1'
#NPM
port2 = '/dev/ttyACM2'

url = 'http://114.71.220.59:7579/Mobius/Ksensor_ubicomp/RPM/la'

brate0 = 9600
brate1 = 9600
brate2 = 9600

headers = {
  'Accept': 'application/json',
  'X-M2M-RI': '12345',
  'X-M2M-Origin': 'SJwrKvZd84v',
  'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

try:
    ser0 = serial.Serial(port0, baudrate=brate0, timeout=None)
    ser1 = serial.Serial(port1, baudrate=brate1, timeout=None)
    ser2 = serial.Serial(port2, baudrate=brate2, timeout=None)
    print('%s and %s and %s' %(ser2.name, ser0.name, ser1.name))
except Exception as err:
    print("Serial err:", err)
    
def send_(time):
    if ser0.in_waiting != 0 and ser1.in_waiting != 0 and ser2.in_waiting != 0:
        try: 
            contect2 = ser2.readline()
            contect0 = ser0.readline()
            contect1 = ser1.readline()
                        
            con0_1 = "{},{},{},{}".format(time, contect2[:-2].decode(), contect0[:-2].decode(), contect1[:-2].decode())
            
        except Exception as err:
             print("Serial read err:", err)

        con0_1 = con0_1.replace("\n", "")
        print(con0_1)
        
        data = "{\n    \"m2m:cin\": {\n        \"con\": \"" + con0_1 + "\"\n    }\n}"

        try:
            r = requests.request("POST", url+"/data", headers=headers, data=data)
            r.raise_for_status()
        except requests.exceptions.RequestException as req_err:
            print("Request error:", req_err)
        except requests.exceptions.HTTPError as http_err:
            print("HTTP error:", http_err)
        except Exception as exc:
            print('There was a problem: %s' % (exc))

previous_date = None

try:
    while True:
        try:
            r = requests.get(url, headers=headers, timeout=5)  # 연결 시간 초과를 10초로 설정
            r.raise_for_status()
            jr = r.json()
            
            # 'm2m:cin'의 'con' 키에서 데이터를 가져옴
            data = jr["m2m:cin"]["con"]
            
            # 데이터를 콤마로 분할하고 첫 번째 요소를 선택하여 날짜 부분만 가져옴
            date = data.split(',')[0]
            
            # 이전 데이터와 새로운 데이터의 날짜가 다르면 새로운 데이터로 갱신하고 출력
            if date != previous_date:
                #print("새로운 데이터 발견! 날짜:", date)
                #start신호를 보낸다.
                send_(date)
                previous_date = date
        except requests.exceptions.ConnectTimeout as e:
            print("연결 시간 초과 오류가 발생했습니다:", e)
        except requests.exceptions.RequestException as e:
            print("요청 오류가 발생했습니다:", e)
        
        time.sleep(6)  # 6초마다 반복

except KeyboardInterrupt:
    print("\n프로그램을 종료합니다.")
except Exception as exc:
    print("문제가 발생했습니다:", exc)
