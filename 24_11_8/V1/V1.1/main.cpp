#include <iostream>
#include <string>
#include <signal.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include "i2c.h"
#include "http.h"
#include "tcp.h"

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
    if (command.substr(3, 1) == "P") {
        action = command.substr(3, 2); // P1 or P2 or P3
    } else {
        action = command.substr(3);
    }

    return true;
}

int main() {
    signal(SIGINT, cleanup);

    // TCP 서버 시작
    TCPServer server(65432);
    server.start();

    int address, arduinoNum;
    std::string action;
    std::string url = "http://114.71.220.59:2021/Mobius/justin/ss/motor";

    std::cout << "start" << std::endl;

    while (true) {
        // TCP로 들어온 데이터를 명령으로 대체
        std::string command = server.getReceivedData();
        // 개행 문자 제거
	command.erase(std::remove(command.begin(), command.end(), '\n'), command.end());
	command.erase(std::remove(command.begin(), command.end(), '\r'), command.end());

        if (command.empty()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        std::cout << "Received command: " << command << std::endl;

        // 명령어 파싱
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

        // I2C 통신 설정
        int file = openI2CDevice(address);
        if (file < 0) continue;

        // 명령에 따른 동작 수행
        if (action == "P1" || action == "P2" || action == "P3") {
            sendCommand(file, command);
        } else if (action == "READ1") {
            std::string receiveData = readData(file, 0);
            std::cout << "Received from Arduino: " << receiveData << std::endl;
            send_value_to_server(url, receiveData);
        } else if (action == "READ2") {
            std::string receiveData = readData(file, 1);
            std::cout << "Received from Arduino: " << receiveData << std::endl;
            send_value_to_server(url, receiveData);
        } else {
            std::cerr << "알 수 없는 명령입니다: " << action << std::endl;
        }

        close(file);
    }

    cleanup(0);
    return 0;
}

