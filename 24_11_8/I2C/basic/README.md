# How To Use

## install lib

```
sudo apt-get update
sudo apt-get install libi2c-dev
```

## 빌드 방법

```
g++ main.cpp i2c.cpp -o main -li2c
```


## 주의사항
- i2c 연결 확인 `sudo i2cdetect -r -y 1` (1은 내가 i2c 2개중 1인곳에 연결함)

