# -*- coding: utf-8 -*-
import requests
import time
import json

data_send_url = 'http://114.71.220.59:2021/Mobius/Ksensor_ubicomp_2/motor'
rpm_url = "http://211.226.18.66:5002/iot/v1/microdust/measure-last"

apm2_headers = {
	'Accept': 'application/json',
	'X-M2M-RI': '12345',
	'X-M2M-Origin': 'SKsensor_ubicomp_2',
	'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

# Global variables
time_s, pm_data = "", ""
last_sent_data, pm_data_save = "", ""

def fetch_data():
	try:
		response = requests.get(rpm_url)
		response.raise_for_status()
		return response.json()
	except requests.HTTPError as http_err:
		print("HTTP error occurred: {} - {}".format(http_err, response.status_code))
		return None
	except Exception as e:
		print("Failed to fetch data: {}".format(e))
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
					pm_data = pm_data_save

					return time_s, pm_data
	else:
		print("No data received")
	return time_s, pm_data
    
while True:
	time_s, pm_data = process_data()
	print(time_s)
	print(pm_data)
	
	con0_1 = "start"
	
	data_apm = json.dumps({
			"m2m:cin": {
			"con": con0_1
		}
	})
    
	try:
		r_apm2 = requests.post(data_send_url, headers=apm2_headers, data=data_apm)
		r_apm2.raise_for_status()
	except requests.exceptions.RequestException as req_err:
		print("APM Request error:", req_err)
	except requests.exceptions.HTTPError as http_err:
		print("APM HTTP error:", http_err)
	except Exception as exc:
		print('APM There was a problem: {}'.format(exc))

	time.sleep(60)
