import requests
import json
import time
import socket

def send_to_mobius(data):
    post_url = 'http://114.71.220.59:7579/Mobius/Ksensor_ubicomp/RPM'
    headers = {
        'Accept': 'application/json',
        'X-M2M-RI': '12345',
        'X-M2M-Origin': 'SqNVGGACjuA',
        'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
    }
    post_data = {
        'm2m:cin': {
            'con': data
        }
    }
    try:
        response = requests.post(post_url, headers=headers, json=post_data)
        response.raise_for_status()
        print(f"Data sent to Mobius server successfully: {data}")
    except requests.HTTPError as http_err:
        print(f"HTTP error occurred: {http_err} - {response.status_code}")
    except Exception as e:
        print(f"Failed to send data to Mobius server: {e}")

def send_meastime_via_tcp(meastime):
    server_address = ('localhost', 9999)  # Adjust as per server configuration
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect(server_address)
            message = json.dumps({"meastime": meastime})
            s.sendall(message.encode())
            print(f"meastime data sent to TCP/IP server successfully: {meastime}")
    except Exception as e:
        print(f"Failed to send meastime data via TCP/IP: {e}")

def fetch_data():
    try:
        response = requests.get('http://211.226.18.66:5002/iot/v1/microdust/measure-last')
        response.raise_for_status()
        return response.json()
    except requests.HTTPError as http_err:
        print(f"HTTP error occurred: {http_err} - {response.status_code}")
        return None
    except Exception as e:
        print(f"Failed to fetch data: {e}")
        return None

def main():
    last_sent_data = None
    try:
        while True:
            data = fetch_data()
            if data:
                for item in data.get('data', []):
                    if item['deviceId'] == '0017B2FFFE50087D':
                        meastime = item.get('meastime', '')
                        if meastime != last_sent_data:
                            formatted_data = f"{meastime},{item.get('temperature', '')},{item.get('humidity', '')},{item.get('pm10', '')},{item.get('pm2.5', '')},{item.get('pm10_a', '')},{item.get('pm2.5_a', '')}"
                            send_to_mobius(formatted_data)
                            send_meastime_via_tcp(meastime)
                            last_sent_data = meastime
            time.sleep(55)  # Adjust delay as necessary
    except KeyboardInterrupt:
        print("\nProgram terminated by user.")

if __name__ == '__main__':
    main()
