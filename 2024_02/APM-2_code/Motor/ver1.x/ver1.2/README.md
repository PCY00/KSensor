# 함수 설명

- misumi(int driverPUL, int driverDIR, long resolution);

  <br>
  
  객체 생성시 생성 ( 핀 값 설정, 모터 분해능 설정, 디폴트 방향 설정을 하는 함수)

---

- void setSpeed(int delaySpeed);
  
  <br>
  
  함수명 대로 모터의 속도를 설정해줌

---

- void move_motor_angle(bool rev);
  
  <br>
  
  모터를 움직이는데 나중에 이 함수에서 각도를 추출할 예정
  rev는 true로 설정시 한바퀴 돌고 다시 반대로 한바퀴돌고 를 반복함
  false로 설정시 한바퀴만 돌고 멈춤

---

- void move_moter(long steps);
  
  <br>
  
  모터를 실질적으로 한스텝을 구동시키는 부분

---

- void revmotor();
  
  <br>
  
  모터를 반대편으로 설정해주고 남은 스텝을 초기화해주는 함수
