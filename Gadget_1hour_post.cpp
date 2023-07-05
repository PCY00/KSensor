#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <chrono>
#include <thread>

#include <curl/curl.h>

using namespace std;

string url_post = "http://203.253.128.177:7579/Mobius/20191546/PMS";

string getValueFromCSV(const string& filename, int row, int col);
void performPOSTRequest(const string& url, const string& post_data);

int main(){
    while(1){
        string value = getValueFromCSV("dust.csv", 0, 2);
        int test = stoi(value);

        string post_data = "{\"m2m:cin\": {\"con\": \"" + to_string(test) + "\"}}";
        performPOSTRequest(url_post, post_data);

        cout << "value: " << value << endl;

        this_thread::sleep_for(chrono::seconds(60*60));
    }

    return 0;
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
