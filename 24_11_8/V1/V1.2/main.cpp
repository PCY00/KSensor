#include <iostream>
#include <string>
#include <signal.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include "i2c.h"
#include "http.h"
#include "tcp.h"

#define SLAVE_ADDRESS_1 0x57    // contain 1 I2C 주소
#define SLAVE_ADDRESS_2 0x58    // contain 2 I2C 주소
#define SLAVE_ADDRESS_3 0x59    // contain 3 I2C 주소
#define SLAVE_ADDRESS_4 0x5A    // Sensors I2C 주소
#define SLAVE_ADDRESS_5 0x5B    // npm I2C 주소

// 프로그램 종료 시 클린업
void cleanup(int signum) {
    std::cout << "\n프로그램을 종료합니다." << std::endl;
    exit(0);
}

// 명령어 파싱 함수
bool parseCommand(const std::string &command, int &arduinoNum, std::string &action) {
    if (command.size() < 4 || command[0] != 'M' || command[2] != '_') return false;

    arduinoNum = command[1] - '0';
    if (command.substr(3, 1) == "P") {
        action = command.substr(3, 2); // P1 or P2 or P3
    } else {
        action = command.substr(3);
    }

    return true;
}

// 현재 시간을 "YYYYMMDDhhmmss" 형식으로 반환하는 함수
std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    // tm 구조체로 변환
    struct tm timeInfo;
    localtime_r(&time, &timeInfo);
    
    // 시간 형식 맞추기: 년월일시분초
    std::stringstream timeStream;
    timeStream << std::setfill('0') << std::setw(4) << (1900 + timeInfo.tm_year)
               << std::setw(2) << (timeInfo.tm_mon + 1)
               << std::setw(2) << timeInfo.tm_mday
               << std::setw(2) << timeInfo.tm_hour
               << std::setw(2) << timeInfo.tm_min
               << std::setw(2) << timeInfo.tm_sec;
    
    return timeStream.str();
}

// 1분마다 data send function
void SendDataEveryMinute(const std::string& apm2_url, const std::string& rpm_url) {
    while (true) {
        std::string APM2_time = getCurrentTime();
        
        //rpm_data_get
        std::string rpm_data_original = get_value_from_server(rpm_url);
        std::cout << "RPM data fetched: " << rpm_data_original << std::endl;
        
// --------------------------------------------------------------------------------------------
        //rpm data process
        std::string RPM_time = "null", RPM_pm_data_b = "null", RPM_pm_data_a = "null";
    
        // rpm_data_original이 비어있지 않으면 데이터를 처리
        if (!rpm_data_original.empty()) {
	    std::stringstream ss(rpm_data_original);  // 문자열을 스트림으로 변환
	    std::string token;  // 쉼표로 구분된 데이터 조각을 저장할 변수
	    std::vector<std::string> rpm_data_original_list;  // 나눠진 데이터를 저장할 벡터

	    // 쉼표를 기준으로 데이터를 나누어 벡터에 추가
	    while (std::getline(ss, token, ',')) {
	        rpm_data_original_list.push_back(token);  // 벡터에 데이터를 추가
	    } 

	    // 벡터의 크기를 확인하고 각 변수에 값 할당
	    if (rpm_data_original_list.size() > 0) RPM_time = rpm_data_original_list[0];  // 첫 번째 데이터는 RPM_time에 할당
	    if (rpm_data_original_list.size() > 5) RPM_pm_data_b = rpm_data_original_list[5];  // 여섯 번째 데이터는 RPM_pm_data_b에 할당
	    if (rpm_data_original_list.size() > 6) RPM_pm_data_a = rpm_data_original_list[6];  // 일곱 번째 데이터는 RPM_pm_data_a에 할당
        }
        
// --------------------------------------------------------------------------------------------
        //read data to arduino (start)
        int file_1 = openI2CDevice(SLAVE_ADDRESS_1);
        std::string receiveData_1 = readData(file_1, 0);
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        std::string receiveData_2 = "0,0,0,0,0,0";
	std::string receiveData_3 = "0,0,0,0,0,0";
	std::string receiveData_4 = "0,0,0,0,0,0,0,0";
	std::string receiveData_5 = "0";
	
	//std::string receiveData_2 = readData(SLAVE_ADDRESS_2, 0);
	//std::string receiveData_3 = readData(SLAVE_ADDRESS_3, 0);
	//std::string receiveData_4 = readData(SLAVE_ADDRESS_4, 0);
	//std::string receiveData_5 = readData(SLAVE_ADDRESS_5, 0);

	// Combined data
	std::string combined_data = APM2_time + "," + RPM_time + "," + receiveData_1 + "," + receiveData_2 + "," + receiveData_3  +  "," + receiveData_4 + "," + receiveData_5  + "," + RPM_pm_data_b + "," + RPM_pm_data_a;
	std::cout << "Combined data: " << combined_data << std::endl;

	// 원래 순서와 원하는 순서를 정의
	std::vector<std::string> original_order = {
		"APM_datetime", "RPM_datetime", "pwm1_1", "pwm1_2", "pwm1_3", "pm1_1", "pm1_2", "pm1_3",
		"pwm2_1", "pwm2_2", "pwm2_3", "pm2_1", "pm2_2", "pm2_3",
		"pwm3_1", "pwm3_2", "pwm3_3", "pm3_1", "pm3_2", "pm3_3",
		"temp", "humi", "o3", "co", "no2", "so2",
		"wind_d", "wind_s", "npm", "rpm before correction", "rpm after correction"
	};

	std::vector<std::string> desired_order = {
		"APM_datetime", "RPM_datetime", "rpm before correction", "rpm after correction",
		"pwm1_1", "pwm1_2", "pwm1_3", "pwm2_1", "pwm2_2", "pwm2_3",
		"pwm3_1", "pwm3_2", "pwm3_3", "npm", "pm1_1", "pm1_2", "pm1_3",
		"pm2_1", "pm2_2", "pm2_3", "pm3_1", "pm3_2", "pm3_3",
		"temp", "humi", "o3", "co", "no2", "so2", "wind_d", "wind_s"
	};

	// original_order의 각 항목의 인덱스를 저장하는 맵 생성
	std::map<std::string, int> index_map;
	for (int i = 0; i < original_order.size(); ++i) {
		index_map[original_order[i]] = i;
	}

	// combined_data를 쉼표로 나누어 데이터 리스트를 생성
	std::vector<std::string> data_list;
	std::stringstream ss(combined_data);
	std::string item;
	while (std::getline(ss, item, ',')) {
		data_list.push_back(item);
	}

	// desired_order에 따라 데이터를 재정렬
	std::vector<std::string> reordered_data;
	for (const auto& col : desired_order) {
		if (index_map.find(col) != index_map.end() && index_map[col] < data_list.size()) {
		    reordered_data.push_back(data_list[index_map[col]]);
		} else {
		    reordered_data.push_back("null"); // 데이터를 찾을 수 없으면 'null' 추가
		}
	}

	// 재정렬된 데이터를 하나의 문자열로 합침
	std::string reordered_combined_data;
	for (size_t i = 0; i < reordered_data.size(); ++i) {
		reordered_combined_data += reordered_data[i];
		if (i < reordered_data.size() - 1) reordered_combined_data += ",";
	}

	// 결과 출력
	std::cout << "Reordered data: " << reordered_combined_data << std::endl;

// --------------------------------------------------------------------------------------------
	//apm2 data send
	send_value_to_server(apm2_url, reordered_combined_data);
	
        // 1분(60초) 동안 대기
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}

int main() {
    signal(SIGINT, cleanup);

    // TCP 서버 시작
    TCPServer server(65432);
    server.start();

    std::string apm2_url = "http://114.71.220.59:2021/Mobius/justin/ss/data";
    std::string rpm_url = "http://114.71.220.59:2021/Mobius/justin/ss/RPMdata/la";
    std::string FanSpeed_url = "http://114.71.220.59:2021/Mobius/justin/ss/fanspeed";
    
    std::cout << "start" << std::endl;

    // SendDataEveryMinute 함수를 별도 스레드로 실행
    std::thread dataSender(SendDataEveryMinute, apm2_url, rpm_url);

    int address, arduinoNum;
    std::string action;

    while (true) {
        std::string command = server.getReceivedData();
        command.erase(std::remove(command.begin(), command.end(), '\n'), command.end());
        command.erase(std::remove(command.begin(), command.end(), '\r'), command.end());

        if (command.empty()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        std::cout << "Received command: " << command << std::endl;

        if (!parseCommand(command, arduinoNum, action)) {
            std::cerr << "잘못된 명령 형식입니다. 형식: M?_ACTION" << std::endl;
            continue;
        }

        if (arduinoNum == 1) {
            address = SLAVE_ADDRESS_1;
        } else if (arduinoNum == 2) {
            address = SLAVE_ADDRESS_2;
        } else {
            std::cerr << "잘못된 아두이노 번호입니다. 1 또는 2를 입력하십시오." << std::endl;
            continue;
        }

        int file = openI2CDevice(address);
        if (file < 0) continue;

        if (action == "P1" || action == "P2" || action == "P3") {
            sendCommand(file, command);
            std::this_thread::sleep_for(std::chrono::seconds(5));
            send_value_to_server(FanSpeed_url, readData(file, 1));
        } else {
            std::cerr << "알 수 없는 명령입니다: " << action << std::endl;
        }

        close(file);
    }

    // dataSender 스레드가 종료될 때까지 대기
    dataSender.join();
    cleanup(0);
    return 0;
}
