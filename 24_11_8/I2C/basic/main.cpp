#include <iostream>
#include <string>
#include <signal.h>
#include <unistd.h>
#include "i2c.h"

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
    action = command.substr(3);

    return true;
}

int main() {
    signal(SIGINT, cleanup);

    std::string command;
    int address, arduinoNum;
    std::string action;

    std::cout << "M?_ON, M?_OFF, M?_D??으로 명령을 줄 수 있습니다.\n?는 아두이노 번호이고, ??는 깜빡이 딜레이(초)입니다." << std::endl;

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

        if (action == "P1_1000" || action == "P1_2000") {
            sendCommand(file, action);
        } else if (action == "READ") {
            readData(file);
        } else {
            std::cerr << "알 수 없는 명령입니다: " << action << std::endl;
        }

        close(file);
    }

    cleanup(0);
    return 0;
}

