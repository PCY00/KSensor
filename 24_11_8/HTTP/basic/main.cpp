#include <iostream>
#include "http.h"
#include <thread>  // std::this_thread::sleep_for
#include <string>


int main() {

	std::string url = "http://114.71.220.59:2021/Mobius/justin/ss/motor";
   	// 서버에 1 값 보내기
    	send_value_to_server(url, "1");

    	// 1초 대기
   	std::this_thread::sleep_for(std::chrono::seconds(1));

    	// 서버에서 값 받기
//	std::string getUrl = url + std::string("/la");
    	std::string received_data = get_value_from_server(url+="/la");
    	std::cout << "Received data: " << received_data << std::endl;

    	return 0;
}

