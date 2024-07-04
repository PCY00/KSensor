# Module

---

## Low Data Print
- pwm1_1,2,3, pm1_1,2,3,pwm2_1,2,3, pm2_1,2,3,pwm3_1,2,3, pm3_1,2,3

---

## How To Using
### Command the following to the serial for fan control
- M1_P?_???
- M2_P?_???
- M3_P?_???

<br>

?에는 숫자가 들어갑니다.
P앞에 숫자는 그 컨테이너의 모듈 넘버입니다.
맨 뒤의 숫자는 0~255값 중 지정된 값을 넣어줍니다.

```
#define Speed_1 40
#define Speed_2 55
#define Speed_3 70
```
