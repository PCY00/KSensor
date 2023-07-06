#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <chrono>
#include <thread>

#include <curl/curl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>

using namespace std;

string url_post = "http://203.253.128.177:7579/Mobius/20191546/PMS";

void unmountUSBDrive();
void mountUSBDrive();
string getValueFromCSV(const string& filename, int row, int col);
void performPOSTRequest(const string& url, const string& post_data);

int main(){
    const char* usbDevicePath = "/dev/loop0";
	const char* mountPoint = "/home/pi/images/usbdisk1.d";

    while(1){
        //데이터 업데이트를 위한 작업
        unmountUSBDrive();
        sleep(1);
        mountUSBDrive();
        //업데이트된 csv파일의 맨 위의 3번쨰 자리 값 읽어오기
        string value = getValueFromCSV("/home/pi/images/usbdisk1.d/KMS.csv", 0, 2);
        int test = stoi(value);

        string post_data = "{\"m2m:cin\": {\"con\": \"" + to_string(test) + "\"}}";
        //서버에 데이터 전송
        performPOSTRequest(url_post, post_data);
        //3600초 대기
        this_thread::sleep_for(chrono::seconds(60*60));
    }
	return 0;
}


void unmountUSBDrive() {
	int unmountResult = system("sudo umount -l /home/pi/images/usbdisk1.d");
	if(unmountResult != 0){
		perror("Unmount failed");
	}else{
		printf("USB drive unmounted successfully.\n");
	}
}

void mountUSBDrive(){
	int mountResult = system("sudo mount -o loop,ro, -t vfat /home/pi/images/usbdisk1.img /home/pi/images/usbdisk1.d");
	if(mountResult != 0){
		perror("Mount failed");
	}else{
		printf("USB drive remounted successfully.\n");
	}
}

void performPOSTRequest(const string& url, const string& post_data) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        // Set the URL to send the POST request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the headers for the POST request
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/vnd.onem2m-res+json; ty=4");
        headers = curl_slist_append(headers, "X-M2M-RI: 12345");
        headers = curl_slist_append(headers, "X-M2M-Origin: SI3oXROBJmB");
        headers = curl_slist_append(headers, "Accept: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());

        // Perform the POST request
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
          fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
      curl_easy_cleanup(curl);
    }
}

string getValueFromCSV(const string& filename, int row, int col){
    ifstream file(filename);
    string value;

    if(file.is_open()){
        string line;
        int currentRow = 0;
        
        while(getline(file, line)){
            if(currentRow == row){
                stringstream ss(line);
                string cell;

                int currentCol = 0;
                while(getline(ss, cell, ',')){
                    if(currentCol == col){
                        value = cell;
                        break;
                    }
                    currentCol++;
                }
                break;
            }
            currentRow++;
        }
        file.close();
    }else{
        cerr << "Failed to open CSV file" << endl;
    }
    return value;
}
