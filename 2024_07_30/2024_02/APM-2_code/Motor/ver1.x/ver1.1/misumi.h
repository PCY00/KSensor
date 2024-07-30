/*
APM-2의 모터를 구동하기 위한 라이브러리 ver1.0
*/

#ifndef misumi_h
#define misumi_h

class misumi
{
    public:
        misumi(int driverPUL, int driverDIR);

        //모터 속도 조절
        void setSpeed(int delaySpeed);
        //모터 방향 전환
        void revmotor();

        //해상도에 따라 한바퀴 test function
        void test_function(int PR);
        
    private:
        //motor pin
        int driverPUL;
        int driverDIR;
        int driverENA;
        int driverALM;

        //motor speed
        int delaySpeed;

        //motor pulse/rev
        int PR;

        //motor save steps
        long leftSteps;

        //pre millis
        unsigned long preMillis;

        //motor diraction
        bool setdir;
};

#endif
