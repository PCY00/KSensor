#ifndef m_3_1_h
#define m_3_1_h

class m_3_1
{
    public:
        m_3_1(int enPin, int stepPin, int dirPin, int swich);

        //reset motor
        void MotorSetup();

        void setspeed(int inputSpeed);
        void moveto();
        void moveStep(long steps);
        void setRound(long inputRound);
        void setDiraction();

    private:
        //motor pin
        int enPin;
        int stepPin;
        int dirPin;

        //switch pin
        int swich;

        //motor speed
        int speeds;

        //motor round
        long rounds;

        //motor save steps
        long leftSteps;

        //pre millis
        unsigned long preMillis;

        //motor diraction
        bool diraction;
};

#endif
