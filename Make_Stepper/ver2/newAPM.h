#ifndef _newAPM_H_
#define _newAPM_H_

#include <Arduino.h>

class newAPM
{
    public:
        newAPM(int stepPin_0, int dirPin_0, int enPin_0, int stepPin_1, int dirPin_1, int enPin_1, int stepPin_2, int dirPin_2, int enPin_2, int stepPin_3, int dirPin_3, int enPin_3, int swich_1, int swich_2, int swich_3);
        void moveto(int stepPin, int speeds);
        void motorTopSetup();
        void run(int speeds, float stepperRound);

    private:
        //bottom
        int stepPin_0;
        int dirPin_0;
        int enPin_0;

        //top
        int stepPin_1;
        int dirPin_1;
        int enPin_1;

        int stepPin_2;
        int dirPin_2;
        int enPin_2;

        int stepPin_3;
        int dirPin_3;
        int enPin_3;

        int swich_1;
        int swich_2;
        int swich_3;
};

#endif
