import serial
import time
import threading

port0 = '/dev/ttyACM0'
port1 = '/dev/ttyACM1'
brate = 9600
a = 0
b = 0
first = 0

# 키보드 입력 감지를 위한 플래그 변수
stop_flag = False

# 임계영역을 위한 Lock 객체 생성
serial_lock = threading.Lock()

# 시리얼 포트 초기화
try:
    ser0 = serial.Serial(port0, baudrate=brate, timeout=5)
    ser1 = serial.Serial(port1, baudrate=brate, timeout=5)
    print('%s and %s' % (ser0.name, ser1.name))
except Exception as err:
    print("Serial err:", err)

# 초기화 시 메시지 전송
if first == 0:
    ser0.write(b'0')
    ser1.write(b'0')
    first = 1
    print("loading....")
    time.sleep(5)

# 시리얼 버퍼 비우기 함수
def clear_serial_buffer():
    ser0.reset_input_buffer()
    ser1.reset_input_buffer()
    print("Serial buffers cleared")

# 키보드 입력 감지 함수
def keyboard_input():
    global stop_flag
    while not stop_flag:
        try:
            user_input = input()
            if user_input == "pcy":
                with serial_lock:  # 임계영역 시작
                    clear_serial_buffer()  # 버퍼 비우기
                    ser0.write(b'pcy')
                    ser1.write(b'pcy')
                    print("Sent 'a' to serial ports")

                    # 5초 대기
                    time.sleep(5)

                    # 시리얼 포트에서 데이터 읽기
                    if ser0.in_waiting > 0:
                        response0 = ser0.readline()
                        print(f"Response from ser0: {response0.decode().strip()}")

                    if ser1.in_waiting > 0:
                        response1 = ser1.readline()
                        print(f"Response from ser1: {response1.decode().strip()}")

            elif user_input == "quit":
                stop_flag = True
                break
        except Exception as e:
            print(f"Error in input thread: {e}")
            stop_flag = True
            break

# 키보드 입력 감지 스레드 시작
input_thread = threading.Thread(target=keyboard_input)
input_thread.start()

try:
    # 메인 루프
    while not stop_flag:
        with serial_lock:  # 임계영역 시작
            print("try")
            clear_serial_buffer()  # 버퍼 비우기
            send = 'start'.encode('utf-8')

            ser0.write(send)
            ser1.write(send)
            time.sleep(2)
            print(send)

            if ser0.in_waiting > 0 and ser1.in_waiting > 0:
                con0 = ser0.readline()
                con1 = ser1.readline()
                a = 1
                b = 1
                print('con0,1 is: %s %s' % (con0, con1))
            else:
                print("0,1 ??")
                time.sleep(2)

        print("sleep")
        a = 0
        b = 0
        time.sleep(10)

except KeyboardInterrupt:
    print("Ctrl + C detected. Exiting...")
    stop_flag = True  # 플래그를 설정하여 스레드 종료 요청
    input_thread.join()  # 스레드가 종료될 때까지 대기

finally:
    if ser0.is_open:
        ser0.close()
    if ser1.is_open:
        ser1.close()
    print("Serial ports closed and program terminated.")

