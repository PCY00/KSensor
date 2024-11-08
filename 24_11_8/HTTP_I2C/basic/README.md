# How To Use

## install lib

```
sudo apt update
sudo apt install libcurl4-openssl-dev libjsoncpp-dev
sudo apt-get install libi2c-dev
```

## 빌드 방법

```
g++ main.cpp i2c.cpp http.cpp -o main -li2c -lcurl -ljsoncpp -std=c++11
```

## 수정사항
- i2c basic 코드에서 아두이노에서 값을 받아오는 것을 출력만 했는데, 따로 string으로 파싱하게 함수를 수정


## 주의사항

- 만약 json.h를 찾지 못할 경우 json.h의 위치를 알아야한다.

```
sudo apt install mlocate
locate json.h
```

- ex) #include <jsoncpp/json/json.h>

- i2c 연결 확인 `sudo i2cdetect -r -y 1` (1은 내가 i2c 2개중 1인곳에 연결함)

