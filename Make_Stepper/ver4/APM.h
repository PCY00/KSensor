#ifndef _APM_H_
#define _APM_H_

#include <Arduino.h>

class APM
{
    public:
        APM(int& stepPin, int& dirPin, int& enPin);
        APM(int& stepPin, int& dirPin, int& enPin, int& swich);
        void motorTopSetup();
        void setting(int& speeds, int& rounds);
        void run();

    private:
        int& stepPin;
        int& dirPin;
        int& enPin;

        int& swich;

        //setting
        int& speeds;
        int& rounds;

        static int step;
};

#endif
