# How To Use

## install lib

```
sudo apt update
sudo apt install libcurl4-openssl-dev libjsoncpp-dev
```

## 빌드 방법

```
g++ main.cpp http.cpp -o main -lcurl -ljsoncpp -std=c++11
```


## 주의사항

- 만약 json.h를 찾지 못할 경우 json.h의 위치를 알아야한다.

```
sudo apt install mlocate
locate json.h
```

- ex) #include <jsoncpp/json/json.h>

