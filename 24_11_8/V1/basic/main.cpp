#include <iostream>
#include <string>
#include <signal.h>
#include <unistd.h>
#include "i2c.h"
#include <thread>
#include "http.h"

#define SLAVE_ADDRESS_1 0x57    // 첫 번째 아두이노의 I2C 주소
#define SLAVE_ADDRESS_2 0x60    // 두 번째 아두이노의 I2C 주소

// 프로그램 종료 시 클린업
void cleanup(int signum) {
    std::cout << "\n프로그램을 종료합니다." << std::endl;
    exit(0);
}

// 명령어 파싱 함수
bool parseCommand(const std::string &command, int &arduinoNum, std::string &action) {
    if (command.size() < 4 || command[0] != 'M' || command[2] != '_') return false;

    arduinoNum = command[1] - '0';
    if(command.substr(3,1) == "P"){
	action = command.substr(3, 2); //P1 or P2 or P3
    }else {
	action = command.substr(3);
    }

    return true;
}

int main() {
    signal(SIGINT, cleanup);

    std::string command;
    int address, arduinoNum;
    std::string action;
    std::string url = "http://114.71.220.59:2021/Mobius/justin/ss/motor";

    std::cout << "start" << std::endl;

    while (true) {
        std::cout << "명령을 입력하세요: ";
        std::cin >> command;

        if (!parseCommand(command, arduinoNum, action)) {
            	std::cerr << "잘못된 명령 형식입니다. 형식: M?_ACTION" << std::endl;
            	continue;
        }

        // 아두이노 번호에 따라 주소 설정
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
        } else if (action == "READ1") {
		std::string recevieData = readData(file, 0);
		std::cout << recevieData << std::endl;
		send_value_to_server(url, recevieData);
        } else if (action == "READ2") {
		std::string recevieData = readData(file, 1);
		std::cout << recevieData << std::endl;
		send_value_to_server(url, recevieData);
	}
	else {
            	std::cerr << "알 수 없는 명령입니다: " << action << std::endl;
        }

        close(file);
    }

    cleanup(0);
    return 0;
}

