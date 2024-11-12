#include <iostream>
#include <thread>
#include <mutex>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const int PORT = 65432;
std::string received_data;
std::mutex data_mutex; // 전역 변수 보호를 위한 mutex

// TCP 서버 스레드 함수
void tcp_server() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return;
    }

    // 포트 재사용 설정
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 바인딩
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return;
    }

    // 대기 시작
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        return;
    }

    while (true) {
        std::cout << "Waiting for connection on port " << PORT << "..." << std::endl;

        // 클라이언트 연결 수락
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            continue;
        }

        std::cout << "Connection accepted." << std::endl;

        // 데이터 수신
        int valread = read(new_socket, buffer, 1024);
        if (valread > 0) {
            std::lock_guard<std::mutex> lock(data_mutex); // 전역 변수 보호
            received_data = std::string(buffer, valread);
            std::cout << "Data received: " << received_data << std::endl;
        }

        // 연결 종료
        close(new_socket);
        std::cout << "Connection closed." << std::endl;
    }

    close(server_fd);
}

// 데이터를 처리하는 함수 (예: 데이터 출력)
void process_data() {
    while (true) {
        {
            std::lock_guard<std::mutex> lock(data_mutex); // 전역 변수 보호
            if (!received_data.empty()) {
                std::cout << "Processing data: " << received_data << std::endl;
                received_data.clear(); // 처리 후 데이터 지우기
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 예제: 1초 간격으로 데이터 확인
    }
}

int main() {
    // TCP 서버 스레드 시작
    std::thread server_thread(tcp_server);

    // 데이터 처리 스레드 시작
    std::thread processing_thread(process_data);

    // 스레드 종료 대기
    server_thread.join();
    processing_thread.join();

    return 0;
}

