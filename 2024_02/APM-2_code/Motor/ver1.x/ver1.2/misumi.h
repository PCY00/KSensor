/*
APM-2의 모터를 구동하기 위한 라이브러리 ver1.0
*/

#ifndef misumi_h
#define misumi_h

class misumi
{
    public:
        misumi(int driverPUL, int driverDIR, long resolution);

        void setSpeed(int delaySpeed);

        void move_motor_angle();
        void move_moter(long steps);
        
        void revmotor();

        //void test_function(int PR);
        
    private:
        //motor pin
        int driverPUL;
        int driverDIR;
        int driverENA;
        int driverALM;

        //motor speed
        int delaySpeed;

        //motor pulse/rev
        long PR;

        //motor save steps
        long leftSteps;

        //pre millis
        unsigned long preMillis;

        //motor diraction
        bool setdir;
};

#endif
